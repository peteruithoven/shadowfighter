/*
 *  VideoInputController.cpp
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 4/13/10.
 *  Copyright 2010 HKU. All rights reserved.
 *
 */

#include "VideoInputController.h"

VideoInputController::VideoInputController()
{
}
void VideoInputController::newPixels(unsigned char * pixels)
{
	analyze(pixels);
}
void VideoInputController::analyze(unsigned char * pixels)
{
	//cout << "VideoInputController::analyze\n";
	// analyze pixels, using configuration stored in model
	
	int videoW = model->videoW;
	int videoH = model->videoH;
	
	// convert color pixels to grayscale pixels
	unsigned char * grayPixels = new unsigned char[videoW*videoH];
	int counter = 0;
	for (int i = 0; i < videoH; i++) {
		for (int j = 0; j < videoW*3; j+=3) {
			
			// get r, g and b components
			int r = (i*videoW*3) + j;
			int g = (i*videoW*3) + (j+1);
			int b = (i*videoW*3) + (j+2);
			
			int grayPixel = (11 * pixels[r] + 16 * pixels[g] + 5 * pixels[b]) / 32;
			
			grayPixels[counter] = grayPixel;
			
			counter++;
		}
	}
	
	model->grayImg->setFromPixels(grayPixels, videoW,videoH);
	
	
	// Optionally store empty frame image
	if (model->willLearnBackground && model->pixelsSource == Model::CAMERA)
	{
		model->grayEmptyImg->setFromPixels(grayPixels,videoW,videoH);
		
		ofImage imgSaver = *new ofImage();
		imgSaver.setFromPixels(grayPixels, videoW, videoH, OF_IMAGE_GRAYSCALE, false);
		imgSaver.saveImage("empty.png");
		
		model->willLearnBackground = false;
	}
	if(model->debugDetection)
		model->grayEmptyImg->draw(videoW, 0);
	
	delete [] grayPixels;
	
	filterProjection();
	
	//if(model->debugDetection)
	//	model->prevGrayDiffImg->draw(0, videoH);
	
	// take the abs value of the difference between background and incoming and then threshold:
	model->grayDiffImg->absDiff(*model->grayEmptyImg, *model->grayImg);
	//model->grayDiffImg->draw(0, videoH);

	model->grayDiffImg->threshold(model->threshold);
	if(model->debugDetection)
		model->grayDiffImg->draw(videoW, videoH);
	
	// store pref grayimage
	model->prevGrayDiffImg->setFromPixels(model->grayDiffImg->getPixels(), videoW, videoH);
	
	contourFinder.findContours(*model->grayDiffImg, model->minBlobArea, model->maxBlobArea, model->maxNumBlobs, true);
	
	float blobDisplayX = videoW;
	float blobDisplayY = videoH;
	int numBlobs = contourFinder.blobs.size();
	//int numPrevBlobs = model->blobs->size();
	
	checkHit();
	
	/*if(model->hitting)
	{
		// draw prev blobs boundingboxes
		if(numPrevBlobs > 0)
		{
			for (int i = 0; i < numPrevBlobs; i++)
			{
				ofxCvBlob * prevBlob = model->blobs->at(i);
				ofNoFill();
				ofSetColor(0x660033);
				ofRectangle boundingRect = prevBlob->boundingRect;
				ofRect(boundingRect.x, 
					   videoH+boundingRect.y, 
					   boundingRect.width, 
					   boundingRect.height);
			}
		}
	}*/
	
	ofRectangle *detectionZone = &model->detectionZone;
	if(model->debugDetection)
	{
		ofNoFill();
		ofSetColor(0x00cc00);
		ofRect(detectionZone->x, detectionZone->y, detectionZone->width, detectionZone->height);
		ofRect(blobDisplayX+detectionZone->x, blobDisplayY+detectionZone->y, detectionZone->width, detectionZone->height);
	}
	// store and draw blobs
	model->blobs->clear();
    for (int i = 0; i < numBlobs; i++)
	{
		ofxCvBlob blob = contourFinder.blobs[i];
		ofRectangle blobRect = blob.boundingRect;
		if(!(blobRect.x >= detectionZone->x &&
			 blobRect.y >= detectionZone->y &&
			 blobRect.x+blobRect.width <= detectionZone->x+detectionZone->width &&
			 blobRect.y+blobRect.height <= detectionZone->y+detectionZone->height))
			continue;
		
		//blob.draw(0,0);
		if(model->debugDetection)
			blob.draw(blobDisplayX,blobDisplayY);
		
		// store copy of blobs
		ofxCvBlob * blobCopy = new ofxCvBlob();
		blobCopy->boundingRect.x = blobRect.x;
		blobCopy->boundingRect.y = blobRect.y;
		blobCopy->boundingRect.width = blobRect.width;
		blobCopy->boundingRect.height = blobRect.height;
		//cout << "  new blob copy: " << blobCopy << "\n";
		model->blobs->push_back(blobCopy);
	}
}

void VideoInputController::filterProjection()
{
	int videoW = model->videoW;
	int videoH = model->videoH;
	
	unsigned char * backgroundPixels = model->grayEmptyImg->getPixels();
	
	// check for projection
	unsigned char * pixels = model->grayImg->getPixels();
	for (int i = 0; i < videoH; i++) {
		for (int j = 0; j < videoW; j++) {
			
			int pixelIndex = (i*videoW) + j;
			if(pixelsSource == CLIP5_DEMO)
				pixels[pixelIndex] -= 30;
			if(pixels[pixelIndex] > 180)
			{
				pixels[pixelIndex] = backgroundPixels[pixelIndex];
			}
		}
	}
	model->grayImg->setFromPixels(pixels, videoW, videoH);
	
	if(model->debugDetection)
		model->grayImg->draw(0,0);
	
	delete [] pixels;
	delete [] backgroundPixels;
}

/*float VideoInputController::getAutoThreshold(ofxCvGrayscaleImage * image)
{
    //cout << "\VideoInputController::getAutoThreshold";
    unsigned char * pixels = image->getPixels();
    int length = image->getWidth()*image->getHeight();
	
    int minLum = 255;
    int maxLum = 0;
    for (int i = 0; i < length; i++) {
        unsigned char pixel = pixels[i];       
        int lum = pixel;
        if(lum<minLum)
            minLum = lum;
        if(lum>maxLum)
            maxLum = lum;
    }
    int threshold = (minLum+maxLum)/2;
	//  cout << "\n    threshold: ";
	//  cout << threshold;
    return threshold;
}*/
/*ofxCvGrayscaleImage* VideoInputController::getBlobImage(ofxCvBlob * blob, ofxCvGrayscaleImage * orgImage)
{
	ofRectangle boundingRect = blob->boundingRect;
	
	ofxCvGrayscaleImage * image = new ofxCvGrayscaleImage();
	image->allocate(boundingRect.width,boundingRect.height);
	
	setPixelsSubRegion(orgImage, image, 
					   boundingRect.x, 
					   boundingRect.y, 
					   boundingRect.width, 
					   boundingRect.height, false);
	
	return image;
}*/
/*void VideoInputController::setPixelsSubRegion(ofxCvImage * orgImage, ofxCvImage * targetImage,int x, int y,int width, int height, bool color)
{
	unsigned char * pixels = orgImage->getPixels();
	int totalWidth = orgImage->getWidth();
	int subRegionLength = width * height;
	if(color) subRegionLength*=3; // rgb
	unsigned char subRegion[subRegionLength];
	
	int result_pix = 0;
	for (int i = y; i < y+height; i++) {
		for(int j = x; j < x+width; j++) {
			int base = (i * totalWidth) + j;
			if(color) base *= 3; // rgb
			
			subRegion[result_pix] = pixels[base];
			result_pix++;
			
			if(color)
			{
				subRegion[result_pix] = pixels[base+1];
				result_pix++;
				subRegion[result_pix] = pixels[base+2];
				result_pix++;
			}
		}
	}
	targetImage->setFromPixels(subRegion, width, height);
}*/
/*ofRectangle VideoInputController::getIntersection(ofRectangle rect1, ofRectangle rect2)
{
	ofRectangle intersection = *new ofRectangle();
	
	int rect1R = rect1.x+rect1.width;
	int rect1B = rect1.y+rect1.height;
	
	int rect2R = rect2.x+rect2.width;
	int rect2B = rect2.y+rect2.height;
	
	int l = (rect1.x < rect2.x)? rect1.x : rect2.x;
	int r = (rect1R < rect2R)? rect1R : rect2R;
	int t = (rect1.y < rect2.y)? rect1.y : rect2.y;
	int b = (rect1B < rect2B)? rect1B : rect2B;
	
	intersection.x = l;
	intersection.y = t;
	intersection.width = r-l;
	intersection.height = b-t;
	
	return intersection;
}*/
/*void VideoInputController::drawRect(ofRectangle rect,int color, int x, int y)
{
	ofNoFill();
	ofSetColor(color);
	ofRect(x+rect.x, 
		   y+rect.y, 
		   rect.width, 
		   rect.height);
}*/
void VideoInputController::checkHit()
{
	//cout << "VideoInputController::checkHit\n";
	int hitBlobDisplayX = 0;
	int hitBlobDisplayY = model->videoH;
	
	model->grayDiffImg->absDiff(*model->grayEmptyImg, *model->grayImg);
	//model->grayDiffImg->draw(0, videoH);
	
	model->grayDiffImg->threshold(model->threshold+model->hitThreshold);
	if(model->debugDetection)
		model->grayDiffImg->draw(hitBlobDisplayX, hitBlobDisplayY);
	
	ofRectangle * detectionZone = &model->hitDetectionZone;
	if(model->debugDetection)
	{
		ofNoFill();
		ofSetColor(0xcc0000);
		ofRect(detectionZone->x, detectionZone->y, detectionZone->width, detectionZone->height);
		ofRect(hitBlobDisplayX+detectionZone->x, hitBlobDisplayY+detectionZone->y, detectionZone->width, detectionZone->height);
	}
	
	hitContourFinder.findContours(*model->grayDiffImg, model->minHitBlobArea, model->maxHitBlobArea, model->maxNumHitBlobs, true);
	int numBlobs = hitContourFinder.blobs.size();
	for (int i = 0; i < numBlobs; i++)
	{
		ofxCvBlob blob = hitContourFinder.blobs[i];
		ofRectangle blobRect = blob.boundingRect;
		
		if(!(blobRect.x >= detectionZone->x &&
			 blobRect.y >= detectionZone->y &&
			 blobRect.x+blobRect.width <= detectionZone->x+detectionZone->width &&
			 blobRect.y+blobRect.height <= detectionZone->y+detectionZone->height))
			continue;
		
		/*if(blobRect.x == detectionZone.x || 
		   blobRect.y == detectionZone.y || 
		   blobRect.x+blobRect.width == detectionZone.x+detectionZone.width ||
		   blobRect.y+blobRect.height == detectionZone.y+detectionZone.height)
			continue;*/
		
		
		   
		
		bool toCloseToPrevHit = false;
		for (int j = 0; j < model->prevHitBlobs->size(); j++)
		{
			ofxCvBlob* prevBlob = model->prevHitBlobs->at(j);
			ofRectangle prevBlobRect = prevBlob->boundingRect;
			
			float dist = ofDist(prevBlobRect.x, prevBlobRect.y, 
								blobRect.x, blobRect.y);
			
			if(dist < model->minDiffHitBlobsPos) 
				toCloseToPrevHit = true;
		}
		if(toCloseToPrevHit)
			continue;
		//blob.draw(hitBlobDisplayX,hitBlobDisplayY);
		if(model->debugDetection)
		{
			blob.draw(0,0);
			blob.draw(hitBlobDisplayX,hitBlobDisplayY);
		}
		
		//model->hitRect = &blobRect;
		model->hitRect->x = blobRect.x;
		model->hitRect->y = blobRect.y;
		model->hitRect->width = blobRect.width;
		model->hitRect->height = blobRect.height;	
		
		model->hit();
	}
	
	if (numBlobs > 0) // && !model->hitting) 
	{
		model->hitting = true;
		// start animation
	}
	else if(numBlobs == 0)
	{
		model->hitting = false;
	}
	
	// store copy of hit blobs
	if(model->prevHitBlobs->size() > 0)
		model->prevHitBlobs->clear();
    for (int i = 0; i < numBlobs; i++)
	{
		ofxCvBlob blob = hitContourFinder.blobs[i];
		ofRectangle blobRect = blob.boundingRect;
		
		ofxCvBlob * blobCopy = new ofxCvBlob();
		blobCopy->boundingRect.x = blobRect.x;
		blobCopy->boundingRect.y = blobRect.y;
		blobCopy->boundingRect.width = blobRect.width;
		blobCopy->boundingRect.height = blobRect.height;
		//cout << "  new blob copy: " << blobCopy << "\n";
		model->prevHitBlobs->push_back(blobCopy);
	}
	
	/*// get prev blobs
	cout << "  get prev blobs\n";
	ofxCvBlob * prevBlob0 = model->blobs->at(0);
	ofxCvBlob * prevBlob1 = model->blobs->at(1);
	
	// determine left & right blob
	cout << "  determine left & right blob\n";
	ofxCvBlob * leftBlob;
	ofxCvBlob * rightBlob;
	if(prevBlob0->boundingRect.x < prevBlob1->boundingRect.x)
	{
		leftBlob = prevBlob0;
		rightBlob = prevBlob1;
	}
	else 
	{
		leftBlob = prevBlob1;
		rightBlob = prevBlob0;
	}
	
	drawRect(leftBlob->boundingRect,0x555555,0,videoH);
	drawRect(rightBlob->boundingRect,0x555555,0,videoH);
	
	// get images inside blobs
	cout << "  get images inside blobs\n";
	ofxCvGrayscaleImage * leftBlobImg = getBlobImage(leftBlob,model->prevGrayDiffImg);
	//leftBlobImg->draw(leftBlob->boundingRect.x,leftBlob->boundingRect.y);
	ofxCvGrayscaleImage * rightBlobImg = getBlobImage(rightBlob,model->prevGrayDiffImg);
	//rightBlobImg->draw(rightBlob->boundingRect.x,rightBlob->boundingRect.y);
	
	// place prevBlobs over newBlob
	cout << "  place prevBlobs over newBlob\n";
	ofxCvBlob newBlob = contourFinder.blobs[0];
	leftBlob->boundingRect.x = newBlob.boundingRect.x;
	rightBlob->boundingRect.x = newBlob.boundingRect.x+newBlob.boundingRect.width-rightBlob->boundingRect.width;
	
	drawRect(leftBlob->boundingRect,0x660033,0,videoH);
	drawRect(rightBlob->boundingRect,0x660033,0,videoH);
	
	// determine overlapping area
	cout << "  determine overlapping area\n";
	ofRectangle intersection = getIntersection(leftBlob->boundingRect,rightBlob->boundingRect);
	ofNoFill();
	ofSetColor(255, 0, 0);
	ofRect(intersection.x,intersection.y,intersection.width,intersection.height);
	
	cout << "  intersection:\n";
	cout << "    intersection.x: " << intersection.x << "\n";
	cout << "    intersection.y: " << intersection.y << "\n";
	cout << "    intersection.width: " << intersection.width << "\n";
	cout << "    intersection.height: " << intersection.height << "\n";
	
	// loop trough intersecting pixels of blobs
	cout << "  loop trough intersecting pixels of blobs\n";
	unsigned char *leftPixels = leftBlobImg->getPixels();
	unsigned char *rightPixels = rightBlobImg->getPixels();
	
	// determine the bounding box of the intersection inside of the images
	cout << "  determine the bounding box of the intersection inside of the images\n";
	int leftInnerX = intersection.x-leftBlob->boundingRect.x;
	int leftInnerY = intersection.y-leftBlob->boundingRect.y;
	int leftInnerR = intersection.x+intersection.width-leftBlob->boundingRect.x;
	int leftInnerB = intersection.y+intersection.height-leftBlob->boundingRect.y;
	
	cout << "  leftInner:\n";
	cout << "    leftInnerX: " << leftInnerX << "\n";
	cout << "    leftInnerY: " << leftInnerY << "\n";
	cout << "    leftInnerR: " << leftInnerR << "\n";
	cout << "    leftInnerB: " << leftInnerB << "\n";
	
	ofNoFill();
	ofSetColor(0, 255, 0);
	ofRect(leftInnerX,leftInnerY,leftInnerR-leftInnerX,leftInnerB-leftInnerY);
	
	int rightInnerX = intersection.x-rightBlob->boundingRect.x;
	int rightInnerY = intersection.y-rightBlob->boundingRect.y;
	int rightInnerR = intersection.x+intersection.width-rightBlob->boundingRect.x;
	int rightInnerB = intersection.y+intersection.height-rightBlob->boundingRect.y;
	
	cout << "  leftInner:\n";
	cout << "    rightInnerX: " << rightInnerX << "\n";
	cout << "    rightInnerY: " << rightInnerY << "\n";
	cout << "    rightInnerR: " << rightInnerR << "\n";
	cout << "    rightInnerB: " << rightInnerB << "\n";
	
	ofNoFill();
	ofSetColor(0, 0, 255);
	ofRect(rightInnerX,rightInnerY,rightInnerR-rightInnerX,rightInnerB-rightInnerY);
	
	// loop trough pixels
	// determine the minX, maxX, minY, maxY
	cout << "  loop trough pixels\n";
	int minX = 0;
	int maxX = 9999;
	int minY = 0;
	int maxY = 9999;
	
	for (int i = intersection.y; i < intersection.height; i++) 
	{
		for(int j = intersection.x; j < intersection.width; j++) 
		{
			//cout << "    " << j << " x " << i << "\n";
			
			int leftPixel = 0;
			if(j >= leftInnerX && j <= rightInnerR && i >= leftInnerY && j <= leftInnerB)
			{
				int leftIndex = (leftInnerY * intersection.width) + leftInnerX;
				
				leftPixel = leftPixels[leftIndex];
				//cout << "    leftPixel" << leftPixel << "\n";
			}
			
			int rightPixel = 0;
			if(j >= rightInnerX && j <= rightInnerR && i >= rightInnerY && j <= rightInnerB)
			{
				int rightIndex = (rightInnerY * intersection.width) + rightInnerX;
				//rightPixel = rightPixels[index];
				rightPixel = rightPixels[rightIndex];
				//cout << "    rightPixel" << rightPixel << "\n";
			}
			
			
			// if both white we got a piece of hitArea
			// determine the minX, maxX, minY, maxY
			if(leftPixel == 255 && rightPixel == 255)
			{
				if(j < minX) minX = j;
				else if(j > maxX) maxX = j;
				if(i < minY) minY = i;
				else if(i > maxY) maxY = i;
			}
		}
	}
	model->hitRect->x = minX;
	model->hitRect->y = minY;
	model->hitRect->width = maxX-minX;
	model->hitRect->height = maxX-minY;
	
	//ofSetFrameRate(1);	*/
}