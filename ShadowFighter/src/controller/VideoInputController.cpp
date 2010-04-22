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
	
	drawBlobsHistory();
	
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
	vector<Blob*> * prevBlobs = new vector<Blob*>;
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
		Blob * blobCopy = new Blob();
		blobCopy->boundingRect.x = blobRect.x;
		blobCopy->boundingRect.y = blobRect.y;
		blobCopy->boundingRect.width = blobRect.width;
		blobCopy->boundingRect.height = blobRect.height;
		//cout << "  new blob copy: " << blobCopy << "\n";
		prevBlobs->push_back(blobCopy);
	}
	model->blobsHistory->push_back(prevBlobs);
	if(model->blobsHistory->size() > model->maxBlobsHistoryLength)
		model->blobsHistory->erase(model->blobsHistory->begin());
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
			if(model->pixelsSource == Model::CLIP5_DEMO)
				pixels[pixelIndex] -= 30;
			if(pixels[pixelIndex] > 255)//180)
			{
				pixels[pixelIndex] = backgroundPixels[pixelIndex];
			}
		}
	}
	model->grayImg->setFromPixels(pixels, videoW, videoH);
	
	if(model->debugDetection)
		model->grayImg->draw(0,0);
}
void VideoInputController::drawRect(ofRectangle rect,int x, int y, int color, int a)
{
	ofEnableAlphaBlending();
	ofNoFill();
	int r = (color >> 16) & 0xff;
	int g = (color >> 8) & 0xff;
	int b = (color >> 0) & 0xff;
	ofSetColor(r,g,b,a);	
	ofRect(x+rect.x, 
		   y+rect.y, 
		   rect.width, 
		   rect.height);
	ofDisableAlphaBlending();
}
void VideoInputController::drawRect(ofRectangle rect,int x, int y, int color)
{
	drawRect(rect, x, y, color, 255);
}
bool sortBlobsOnX(ofxCvBlob * blob1, ofxCvBlob * blob2)
{
	return (blob1->boundingRect.x < blob2->boundingRect.x); 
}
void VideoInputController::drawBlobsHistory()
{
	int blobsHistoryDisplayX = model->videoW*2;
	int blobsHistoryDisplayY = 0;
	
	if(model->debugDetection)
		model->prevGrayDiffImg->draw(blobsHistoryDisplayX,blobsHistoryDisplayY);
	
	int blobColors[4] = {0xff0000, 0x00ff00, 0x0000ff, 0xffff00};
	int historyLength = model->blobsHistory->size();
	if(historyLength > 0)
	{
		int initAlpha = 50;
		int alphaStep = (255-initAlpha)/historyLength;
		for (int i = 0; i < historyLength; i++)
		{
			vector<Blob*> * blobs = model->blobsHistory->at(i);
			
			if(i == 0)
				std::sort (blobs->begin(), blobs->end(), &sortBlobsOnX);
			
			for (int j = 0; j < blobs->size(); j++)
			{
				int alpha = initAlpha + alphaStep*i;
				Blob * blob = blobs->at(j);
				
				if(i == 0)
				{
					blob->color = blobColors[j];
				}
				else
				{
					vector<Blob*> * prevBlobs = model->blobsHistory->at(i-1);
					blob->color = 0xffffff;
					for (int l = 0; l < prevBlobs->size(); l++)
					{
						Blob * prevBlob = prevBlobs->at(l);
						if(matchBlobs(blob, prevBlob))
							blob->color = prevBlob->color;
					}
				}
				
				drawRect(blob->boundingRect,blobsHistoryDisplayX,blobsHistoryDisplayY,blob->color,alpha);
			}
		}
	}
	ofSetColor(0xffffff);
}

bool  VideoInputController::matchBlobs(ofxCvBlob * blob1, ofxCvBlob * blob2)
{
	//cout << "VideoInputController::matchBlobs\n";
	ofRectangle blobRect1 = blob1->boundingRect;
	ofRectangle blobRect2 = blob2->boundingRect;
	
	int blob1CX = blobRect1.x+blobRect1.width/2;
	int blob1CY = blobRect1.y+blobRect1.height/2;
	int blob2CX = blobRect2.x+blobRect2.width/2;
	int blob2CY = blobRect2.y+blobRect2.height/2;
	
	int dis = ofDist(blob1CX,blob1CY,blob2CX,blob2CY);
	
	bool match = (dis < model->blobDiffTolerance);
	return match;
}
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
		
		//if(!thereWasAnAttack())
		//	continue;
		
		
		
		
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
bool VideoInputController::thereWasAnAttack()
{
	cout << "VideoInputController::thereWasAnAttack\n";
	
	int blobDiffTolerance = 100;
	int minAttackSpeed = 0;
	
	bool isAttack = false;
	int historyLength = model->blobsHistory->size();
	if(historyLength < model->maxBlobsHistoryLength)
		return false;
	//for (int i = historyLength-1; i > 0; i--)
	//{
		//cout << "  i: " << i << "\n";
		vector<Blob*> * blobs = model->blobsHistory->at(historyLength-1);
		vector<Blob*> * prevBlobs = model->blobsHistory->at(historyLength-3);
		for (int j = 0; j < blobs->size(); j++)
		{
			Blob * blob = blobs->at(j);
			ofRectangle blobRect = blob->boundingRect;
			for (int l = 0; l < prevBlobs->size(); l++)
			{
				Blob * prevBlob = prevBlobs->at(l);
				ofRectangle prevBlobRect = prevBlob->boundingRect;
				
				
				int diffX = prevBlobRect.x-blobRect.x;
				if(diffX < 0) diffX *= -1;
				cout << "  diffX: " << diffX << "\n";
				int diffY = prevBlobRect.y-blobRect.y;
				if(diffY < 0) diffY *= -1;
				cout << "  diffY: " << diffY << "\n";
				int diffWidth = prevBlobRect.width-blobRect.width;
				if(diffWidth < 0) diffWidth *= -1;
				cout << "  diffWidth: " << diffWidth << "\n";
				int diffHeight = prevBlobRect.height-blobRect.height;
				if(diffHeight < 0) diffHeight *= -1;
				cout << "  diffHeight: " << diffHeight << "\n";
				
				if(diffX < blobDiffTolerance && 
				   diffY < blobDiffTolerance && 
				   diffWidth < blobDiffTolerance && 
				   diffHeight < blobDiffTolerance)
				{
					cout << "  found matching prev blob\n";
					
					cout << "    prevBlobRect.x: " << prevBlobRect.x << "\n";
					cout << "    blobRect.x: " << blobRect.x << "\n";
					cout << "    diffX: " << diffX << "\n";
					
					// same blob, prev frame
					int attackSpeed;
					bool onLeftSide = (blobRect.x+blobRect.width < model->videoW/2);
					cout << "    onLeftSide: " << onLeftSide << "\n";
					if(onLeftSide)
						attackSpeed = (blobRect.x+blobRect.width)-(prevBlobRect.x+prevBlobRect.width);
					else 
						attackSpeed = (prevBlobRect.x-blobRect.x)*-1;
					cout << "    attackSpeed: " << attackSpeed << "\n";
					if(attackSpeed >= minAttackSpeed || attackSpeed <= -minAttackSpeed)
						isAttack = true;
				}
			}
		//}
	}
	
	/*
	
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
	}*/
	//return isAttack;
	return true;
}
