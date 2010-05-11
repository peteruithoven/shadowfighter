/*
 *  VideoInputController.cpp
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 4/13/10.
 *  Copyright 2010 HKU. All rights reserved.
 *
 */

#include "VideoInputController.h"
#include "ofxCvColorImage.h"

VideoInputController::VideoInputController()
{
	font.loadFont("mono.ttf", 10);
}
bool sortBlobsOnX(ofxCvBlob * blob1, ofxCvBlob * blob2)
{
	return (blob1->boundingRect.x < blob2->boundingRect.x); 
}
void VideoInputController::newPixels(unsigned char * pixels)
{
	if(model->state != STATE_GAME_FINISHED)
		analyze(pixels);
}
void VideoInputController::analyze(unsigned char * pixels)
{
	//cout << "VideoInputController::analyze\n";
	model->frameCounter++;
	
	int videoW = model->videoW;
	int videoH = model->videoH;
	
	// convert color pixels to grayscale pixels
	unsigned char * grayPixels = toGrayscale(pixels);
	model->grayImg->setFromPixels(grayPixels, videoW,videoH);
	// Optionally store empty frame image
	if (model->willLearnBackground && model->pixelsSource == Model::CAMERA)
	{
		storeBackgroundImage(grayPixels);
		model->willLearnBackground = false;
	}
	
	correctEmptyImage();
	
	if(model->debugDetection)
		model->grayEmptyImg->draw(videoW, 0);
	delete [] grayPixels;
	
	// filters
	filterProjection();
	
	findHitBlobs();
	
	findShadowBlobs();
	
	
	if(model->debugDetection)
		drawBlobsHistory();
	
	if(model->state == STATE_GAME)
	{
		if(model->simpleHitBlobAnalysis)
			analyseHitBlobsSimple(pixels);
		else
			analyseHitBlobs();
	}
	else 
	{
		analyzeShadowsForPlayers();
	}
	
	storeShadowBlobs();

	storeHistory();
	
	takeScreenShot("");
}
void VideoInputController::storeBackgroundImage(unsigned char * grayPixels)
{
	//cout << "VideoInputController::storeBackgroundImage\n";
	model->grayEmptyImg->setFromPixels(grayPixels,model->videoW,model->videoH);
	
	ofImage imgSaver = *new ofImage();
	imgSaver.setFromPixels(grayPixels, model->videoW, model->videoH, OF_IMAGE_GRAYSCALE, false);
	imgSaver.saveImage("empty.png");
}
void VideoInputController::filterProjection()
{
	//cout << "VideoInputController::filterProjection\n";
	int videoW = model->videoW;
	int videoH = model->videoH;
	
	unsigned char * backgroundPixels = model->grayEmptyImg->getPixels();
	
	// check for projection
	unsigned char * pixels = model->grayImg->getPixels();
	for (int i = 0; i < videoH; i++) {
		for (int j = 0; j < videoW; j++) {
			
			int pixelIndex = (i*videoW) + j;
			if(model->pixelsSource == Model::CLIP5_DEMO)
			{
				pixels[pixelIndex] -= 30;
			}
			/*else if(model->pixelsSource == Model::CLIP6_DEMO)
			{
				//cout << "pixels[pixelIndex]: " << pixels[pixelIndex] << "\n";
				pixels[pixelIndex] -= model->clip6Correction; //49;
				//cout << "->pixels[pixelIndex]: " << pixels[pixelIndex] << "\n";
				//if(pixels[pixelIndex] < 0)
					//pixels[pixelIndex] = 0;
			}*/
			if(pixels[pixelIndex] > 180)
			{
				pixels[pixelIndex] = backgroundPixels[pixelIndex];
			}
		}
	}
	model->grayImg->setFromPixels(pixels, videoW, videoH);
	
	//cout << "  model->clip6Correction: " << model->clip6Correction << "\n";
	
	//if(model->debugDetection)
	//	model->grayImg->draw(0,0);
}
void VideoInputController::correctEmptyImage()
{
	if(model->pixelsSource != Model::CLIP6_DEMO)
		return;
	
	int videoW = model->videoW;
	int videoH = model->videoH;
	
	unsigned char * orgPixels = model->grayEmptyCopyImg->getPixels();
	//model->grayEmptyImg->setFromPixels(orgPixels, videoW, videoH);
	
	//unsigned char * pixels = model->grayEmptyCopyImg->getPixels();
	
	unsigned char* newPixels = new unsigned char[videoW*videoH];
	
	for (int i = 0; i < videoH; i++) 
	{
		for (int j = 0; j < videoW; j++) 
		{
			int pixelIndex = (i*videoW) + j;
			newPixels[pixelIndex] = orgPixels[pixelIndex] + model->clip6EmptyCorrection;
		}
	}
	model->grayEmptyImg->setFromPixels(newPixels, videoW, videoH);
	delete [] newPixels;
}
void VideoInputController::findHitBlobs()
{
	//cout << "VideoInputController::findHitBlobs\n";
	int hitBlobDisplayX = 0;
	int hitBlobDisplayY = model->videoH;
	
	model->grayHitDiffImg->absDiff(*model->grayEmptyImg, *model->grayImg);
	//model->grayHitDiffImg->draw(0, videoH);
	
	model->grayHitDiffImg->threshold(model->threshold+model->hitThreshold);
	if(model->debugDetection)
		model->grayHitDiffImg->draw(hitBlobDisplayX, hitBlobDisplayY);
	
	hitContourFinder.findContours(*model->grayHitDiffImg, model->minHitBlobArea, model->maxHitBlobArea, model->maxNumHitBlobs, true);
	//hitContourFinder.findContours(*model->grayHitDiffImg, 5, 640*480, 1000, false);
}
void VideoInputController::findShadowBlobs()
{
	// take the abs value of the difference between background and incoming and then threshold:
	model->grayDiffImg->absDiff(*model->grayEmptyImg, *model->grayImg);
	model->grayDiffImg->threshold(model->threshold);
	
	
	
	// loop through pixels of grayHitDiffImg if white, 
	//   paint same pixel index in grayDiffImg black
	//cout << "VideoInputController::filterProjection\n";
	int videoW = model->videoW;
	int videoH = model->videoH;
	
	//unsigned char * grayHitDiffPixels = model->grayHitDiffImg->getPixels();
	//unsigned char * grayDiffPixels = model->grayDiffImg->getPixels();
	/*for (int i = 0; i < videoH; i++) {
		for (int j = 0; j < videoW; j++) {
			
			int pixelIndex = (i*videoW) + j;
			
			if(grayHitDiffPixels[pixelIndex] == 255)
			{
				int tPI = ((i-1)*videoW) + j;
				int bPI = ((i+1)*videoW) + j;
				int lPI = (i*videoW) + (j-1);
				int rPI = (i*videoW) + (j+1);
				
				if(tPI >= 0) grayHitDiffPixels[tPI] = 200;
				if(bPI >= 0) grayHitDiffPixels[bPI] = 200;
				if(lPI >= 0) grayHitDiffPixels[lPI] = 200;
				if(rPI >= 0) grayHitDiffPixels[rPI] = 200;
			}
		}
	}
	for (int i = 0; i < videoH; i++) {
		for (int j = 0; j < videoW; j++) {
			
			int pixelIndex = (i*videoW) + j;
			
			if(grayHitDiffPixels[pixelIndex] == 200)
			{
				grayHitDiffPixels[pixelIndex] = 255;
			}
		}
	}*/
	/*for (int i = 0; i < videoH; i++) {
		for (int j = 0; j < videoW; j++) {
			
			int pixelIndex = (i*videoW) + j;
			
			if(grayHitDiffPixels[pixelIndex] == 255)
				grayDiffPixels[pixelIndex] = 0;
		}
	}
	model->grayDiffImg->setFromPixels(grayDiffPixels, videoW, videoH);*/
	
	/*ofRectangle * hitDetectionZone = &model->hitDetectionZone;
	int increase = 10;
	for(int i = 0; i<hitContourFinder.blobs.size();i++)
	{
		ofxCvBlob hitBlob = hitContourFinder.blobs[i];
		ofRectangle hitBlobRect = hitBlob.boundingRect;
		
		if((hitBlobRect.x >= hitDetectionZone->x &&
			 hitBlobRect.y >= hitDetectionZone->y &&
			 hitBlobRect.x+hitBlobRect.width <= hitDetectionZone->x+hitDetectionZone->width &&
			 hitBlobRect.y+hitBlobRect.height <= hitDetectionZone->y+hitDetectionZone->height))
		{
			hitBlobRect.x -= increase/2;
			hitBlobRect.y -= increase/2;
			hitBlobRect.width += increase;
			hitBlobRect.height += increase;
			colorInImage(model->grayDiffImg, 0, hitBlobRect);
		}
	}*/
	
	
	if(model->pixelsSource == Model::CLIP6_DEMO)
	{
		ofRectangle colorRect;
		colorRect.width = model->videoW;
		colorRect.height = 50;
		colorRect.x = 0;
		colorRect.y = model->videoH-colorRect.height;
		colorInImage(model->grayDiffImg, 0x000000, colorRect);
	}
	
	if(model->debugDetection)
		model->grayDiffImg->draw(model->videoW, model->videoH);
	// find shadow/normal blobs
	contourFinder.findContours(*model->grayDiffImg, model->minBlobArea, model->maxBlobArea, model->maxNumBlobs, true);
	
	
	float blobDisplayX = model->videoW;
	float blobDisplayY = model->videoH;
	ofRectangle *detectionZone = &model->detectionZone;
	if(model->debugDetection)
	{
		ofNoFill();
		ofSetColor(0x00cc00);
		ofRect(detectionZone->x, detectionZone->y, detectionZone->width, detectionZone->height);
		ofRect(blobDisplayX+detectionZone->x, blobDisplayY+detectionZone->y, detectionZone->width, detectionZone->height);
	}
	
	//model->currentBlobs->clear();
	vector<Blob*> * currentBlobs = new vector<Blob*>; 
    for (int i = 0; i < contourFinder.blobs.size(); i++)
	{
		ofxCvBlob blob = contourFinder.blobs[i];
		ofRectangle blobRect = blob.boundingRect;
		/*if(!(blobRect.x >= detectionZone->x &&
		 blobRect.y >= detectionZone->y &&
		 blobRect.x+blobRect.width <= detectionZone->x+detectionZone->width &&
		 blobRect.y+blobRect.height <= detectionZone->y+detectionZone->height))
		 continue;*/
		
		/*int blobRectCX = blobRect.x+blobRect.width/2;
		 int blobRectCY = blobRect.y+blobRect.height/2;
		 if(!(blobRectCX >= detectionZone->x &&
		 blobRectCY >= detectionZone->y &&
		 blobRectCX <= detectionZone->x+detectionZone->width &&
		 blobRectCY <= detectionZone->y+detectionZone->height))
		 continue;*/
		
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
		//model->currentBlobs->push_back(blobCopy);
		currentBlobs->push_back(blobCopy);
	}
	model->blobsHistory->push_back(currentBlobs);
	if(model->blobsHistory->size() > model->maxBlobsHistoryLength)
	{
		vector<Blob*> * blobs = model->blobsHistory->at(0);
		delete blobs;
		model->blobsHistory->erase(model->blobsHistory->begin());
	}
}
void VideoInputController::storeShadowBlobs()
{	
	//model->blobsHistory->push_back(model->currentBlobs);
	//if(model->blobsHistory->size() > model->maxBlobsHistoryLength)
	//	model->blobsHistory->erase(model->blobsHistory->begin());
}
void VideoInputController::storeHistory()
{
	//cout << "VideoInputController::storeHistory\n";
	// store copy of hit blobs
	if(model->prevHitBlobs->size() > 0)
	{
		for (int i = 0; i < model->prevHitBlobs->size(); i++)
		{
			ofxCvBlob * blob = model->prevHitBlobs->at(i);
			delete blob;
		}
		model->prevHitBlobs->clear();
	}
    for (int i = 0; i < hitContourFinder.blobs.size(); i++)
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
	
	// store pref grayimage
	model->prevGrayDiffImg->setFromPixels(model->grayDiffImg->getPixels(), model->videoW, model->videoH);
	
}

void VideoInputController::drawBlobsHistory()
{
	//cout << "VideoInputController::drawBlobsHistory\n";
	int blobsHistoryDisplayX = model->videoW*2;
	int blobsHistoryDisplayY = 0;
	
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
void VideoInputController::analyseHitBlobsSimple(unsigned char * colorPixels)
{
	cout << "VideoInputController::analyseHitBlobsSimple\n";
	// per hit blob
	// check detection area
	// check unique
	// hittest check with current shadowblob
	// analyze left / right
	// correct with prev shadowblobs (if 2) 
	// human shadow hit filter ?
	// check hit blob speed
	
	int videoW = model->videoW;
	int videoH = model->videoH;
	
	int hitBlobDisplayX = 0;
	int hitBlobDisplayY = videoH;
	int attacksDisplayX = videoW*2;
	int attacksDisplayY = videoH;
	
	ofRectangle * detectionZone = &model->hitDetectionZone;
	if(model->debugDetection)
	{
		ofNoFill();
		ofSetColor(0xcc0000);
		ofRect(detectionZone->x, detectionZone->y, detectionZone->width, detectionZone->height);
		ofRect(hitBlobDisplayX+detectionZone->x, hitBlobDisplayY+detectionZone->y, detectionZone->width, detectionZone->height);
		
		int centerX = model->centerX;
		
		ofNoFill();
		ofSetColor(0xffffff);
		ofLine(centerX, 0, centerX, videoH);
		ofLine(hitBlobDisplayX+centerX, hitBlobDisplayY+0, hitBlobDisplayX+centerX, hitBlobDisplayY+videoH);
	}
	
	//cout << "hitContourFinder.blobs.size(): " << hitContourFinder.blobs.size() << "\n";
	
	if(hitContourFinder.blobs.size() == 0) return;
	
	model->hitsTextX = hitBlobDisplayX;
	model->hitsTextY = hitBlobDisplayY;
	
	drawHitText("  Found hit blobs ");
	
	model->hitCounter = 0;
	for (int i = 0; i < hitContourFinder.blobs.size(); i++)
	{
		ofxCvBlob hitBlob = hitContourFinder.blobs[i];
		ofRectangle hitBlobRect = hitBlob.boundingRect;
		
		model->hitCounter++;

		if(model->debugDetection)
		{
			drawHitText("  Found hit blob");
			hitBlob.draw(0,0);
			hitBlob.draw(hitBlobDisplayX,hitBlobDisplayY);
		}
		
		/*if(!isCorrectColor(hitBlobRect,colorPixels))
		{
			drawHitText("  Hit blob doesn't have the right color");
			takeHitScreenShot("("+ofToString(model->hitCounter)+")");
			continue;
		}	
		drawHitText("  Hit blob has the right color");*/
		
		int hitBlobXC = hitBlobRect.x+hitBlobRect.width/2;
		int hitBlobYC = hitBlobRect.y+hitBlobRect.height/2;
		if(!(hitBlobXC >= detectionZone->x &&
			 hitBlobYC >= detectionZone->y &&
			 hitBlobXC <= detectionZone->x+detectionZone->width &&
			 hitBlobYC <= detectionZone->y+detectionZone->height))
		{
			drawHitText("  Hit blob is outside detectionZone");
			continue;
		}
		drawHitText("  Hit blob is inside detectionZone");
		
		if(!hitIsUnique(hitBlobRect))
		{
			drawHitText("  Hit blob isn't a unique hit blob");
			takeHitScreenShot("("+ofToString(model->hitCounter)+")");
			continue;
		}
		drawHitText("  Hit blob is a unique hit blob");
		
		
		
		
		
		// hittest with current shadow blobs
		bool hitsShadow = false;
		for (int j = 0; j < contourFinder.blobs.size(); j++)
		{
			ofxCvBlob shadowBlob = contourFinder.blobs[j];
			ofRectangle shadowBlobRect = shadowBlob.boundingRect;
			if(rectHitTest(hitBlobRect,shadowBlobRect))
				hitsShadow = true;
		}
		
		if(!hitsShadow)
		{
			drawHitText("  Hit blob doesn't hit shadow blobs");
			takeHitScreenShot("("+ofToString(model->hitCounter)+")");
			continue;
		}	
		drawHitText("  Hit blob hits shadow blob(s)");
		
		
		
		
		
		
		
		
		
		   
		
		
		
		
		
		
		
		
		
		
		
		
		
		int hitBlobRectCX = hitBlobRect.x+hitBlobRect.width/2;
		int deadZoneWidth = 20;
		int deadZoneL = model->centerX-deadZoneWidth/2;
		int deadZoneR = model->centerX+deadZoneWidth/2;
		
		if(hitBlobRectCX > deadZoneL && hitBlobRectCX < deadZoneR)
		{
			drawHitText("  Hit blob inside deadzone");
			takeHitScreenShot("("+ofToString(model->hitCounter)+")");
			continue;
		}
		drawHitText("  Hit blob outside deadzone");
		
		if(!hitsBody(hitBlobRect))
		{
			drawHitText("  Hit blob doesn't hit the body");
			takeHitScreenShot("("+ofToString(model->hitCounter)+")");
			continue;
		}
		drawHitText("  Hit blob hit's the body");
		
		
		
		
		   
		   
		   
		int victim = (hitBlobRectCX < model->centerX)? 1 : 2;
		
		
		/*int attackSpeed = 0;
		int numPrevHitBlobs = model->prevHitBlobs->size();
		for (int j = 0; j < numPrevHitBlobs; j++)
		{
			ofxCvBlob* prevHitBlob = model->prevHitBlobs->at(j);
			if(matchBlobs(&hitBlob, prevHitBlob))
			{
				ofRectangle prevHitBlobRect = prevHitBlob->boundingRect;
				attackSpeed = hitBlobRect.x-prevHitBlobRect.x;
				drawHitText("  attackSpeed: " + ofToString(attackSpeed));
			}
			//ofRectangle prevHitBlobRect = prevHitBlob->boundingRect;
			//int prevHitBlobRect.x
			
			cout << "  attackSpeed: " << attackSpeed << "\n";
			
			// if hit blobs are to close to a prev hit blob they are not unique
		}
		if(attackSpeed > -5 && victim == 1)
			continue;
		else if(attackSpeed < 5 && victim == 2)
			continue;*/
		
		
		
		
		
		
		
		
		// TODO: correct with prev shadowblobs (if 2) 
		// TODO: human shadow hit filter ?
		// TODO: check hit blob speed (find old, match blobs, check speed) 
		//			(store if hit blobs are valid, store only valid blobs)
		model->hitRect->x = hitBlobRect.x;
		model->hitRect->y = hitBlobRect.y;
		model->hitRect->width = hitBlobRect.width;
		model->hitRect->height = hitBlobRect.height;	
		
		drawHitText("  victim: "+ofToString(victim));		
		model->hit(0,0,victim);
		
		drawHitText("  healths: "+ofToString(model->player1Health)+" vs. "+ofToString(model->player2Health));
		takeHitScreenShot("("+ofToString(model->hitCounter)+")");
	}
}
void VideoInputController::analyseHitBlobs()
{
	//cout << "VideoInputController::analyseHitBlobs (frame: "<<model->frameCounter<<")\n";
	
	if(hitContourFinder.blobs.size() == 0) return;
	
	int hitBlobDisplayX = 0;
	int hitBlobDisplayY = model->videoH;
	int attacksDisplayX = model->videoW*2;
	int attacksDisplayY = model->videoH;
	
	model->hitsTextX = hitBlobDisplayX;
	model->hitsTextY = hitBlobDisplayY;
	
	
	drawHitText("  Found hit blobs ");
	
	ofRectangle * detectionZone = &model->hitDetectionZone;
	if(model->debugDetection)
	{
		ofNoFill();
		ofSetColor(0xcc0000);
		ofRect(detectionZone->x, detectionZone->y, detectionZone->width, detectionZone->height);
		ofRect(hitBlobDisplayX+detectionZone->x, hitBlobDisplayY+detectionZone->y, detectionZone->width, detectionZone->height);
	}
	int validBlobs = 0;
	for (int i = 0; i < hitContourFinder.blobs.size(); i++)
	{
		ofxCvBlob hitBlob = hitContourFinder.blobs[i];
		ofRectangle hitBlobRect = hitBlob.boundingRect;
		
		if(!(hitBlobRect.x >= detectionZone->x &&
			 hitBlobRect.y >= detectionZone->y &&
			 hitBlobRect.x+hitBlobRect.width <= detectionZone->x+detectionZone->width &&
			 hitBlobRect.y+hitBlobRect.height <= detectionZone->y+detectionZone->height))
		{
			validBlobs++;
		}
	}
	if(validBlobs <= 0)
	{
		drawHitText("  0 hit blobs inside detection area.");
		return;
	}
	
	// if there where not 2 blobs before this frame, we can't check for an attack, so cancel
	vector<vector<Blob*> * > * blobsHistory = model->blobsHistory;
	int historyLength = blobsHistory->size();
	if(blobsHistory->size() < 1) 
	{
		drawHitText("  There is not enough blob history to detect attacks");
		takeHitScreenShot("("+ofToString(model->hitCounter)+")");
		return;
	}
	drawHitText("  There is enough blob history to detect attacks");

	vector<Blob*> * prevBlobs = blobsHistory->at(blobsHistory->size()-1);
	if(prevBlobs->size() < 2)
	{
		drawHitText("  There weren't 2 blobs in prev blobs\n");
		takeHitScreenShot("("+ofToString(model->hitCounter)+")");
		return;
	}
	drawHitText("  There were 2 blobs in prev blobs\n");
	
	model->hitCounter = 0;
	for (int i = 0; i < hitContourFinder.blobs.size(); i++)
	{
		ofxCvBlob hitBlob = hitContourFinder.blobs[i];
		ofRectangle hitBlobRect = hitBlob.boundingRect;
		
		model->hitCounter++;
		model->attacksTextX = attacksDisplayX;
		model->attacksTextY = attacksDisplayY;
		
		if(model->debugDetection)
		{
			drawHitText("  Found hit blob");
			hitBlob.draw(0,0);
			hitBlob.draw(hitBlobDisplayX,hitBlobDisplayY);
		}
		
		if(!(hitBlobRect.x >= detectionZone->x &&
			 hitBlobRect.y >= detectionZone->y &&
			 hitBlobRect.x+hitBlobRect.width <= detectionZone->x+detectionZone->width &&
			 hitBlobRect.y+hitBlobRect.height <= detectionZone->y+detectionZone->height))
		{
			drawHitText("  Hit blob is outside detectionZone");
			continue;
		}
		drawHitText("  Hit blob is inside detectionZone");
		
		if(!hitIsUnique(hitBlobRect))
		{
			drawHitText("  Hit blob isn't a unique hit blob");
			takeHitScreenShot("("+ofToString(model->hitCounter)+")");
			continue;
		}
		drawHitText("  Hit blob is a unique hit blob");
		
		//cout << "VideoInputController::there was an attack? (" << model->possibleAttacksCounter << ")\n";
		
		// Analyze possible attack
		string counters = ofToString(model->hitCounter)+"/"+ofToString(model->possibleAttacksCounter);
		
		//int blobDiffTolerance = 100;
		int minAttackSpeed = model->minAttackSpeed;
		//cout << "  minAttackSpeed: " << minAttackSpeed << "\n";
		bool isAttack = false;
		
		drawAttackText("there was an attack? ("+counters+")");
		
		ofSetColor(0xffffff);
		if(model->debugDetection)
			model->prevGrayDiffImg->draw(attacksDisplayX,attacksDisplayY);
		
		int historyLength = model->blobsHistory->size();
		if(historyLength < 2) 
		{
			drawAttackText("  not enough blobs history");
			takeHitScreenShot("("+ofToString(model->hitCounter)+")");
			return; 
		}
		drawAttackText("  enough blobs history");
		
		vector<Blob*> * prevBlobs = model->blobsHistory->at(historyLength-1);
		std::sort (prevBlobs->begin(), prevBlobs->end(), &sortBlobsOnX);
		
		int blobColors[4] = {0xff0000, 0x00ff00, 0x0000ff, 0xffff00};
		for (int i = 0; i < prevBlobs->size(); i++)
		{
			Blob * prevBlob = prevBlobs->at(i);
			ofRectangle prevBlobRect = prevBlob->boundingRect;
			prevBlob->color = blobColors[i];
			
			vector<Blob*> * prevPrevBlobs = model->blobsHistory->at(historyLength-2);
			if(prevPrevBlobs->size() < 2) 
			{
				drawAttackText("  not enough prev prev blobs");
				takeHitScreenShot("("+ofToString(model->hitCounter)+")");
			}
			for (int j = 0; j < prevPrevBlobs->size(); j++)
			{
				Blob * prevPrevBlob = prevPrevBlobs->at(j);
				if(matchBlobs(prevBlob, prevPrevBlob))
				{
					ofRectangle prevPrevBlobRect = prevPrevBlob->boundingRect;
					prevPrevBlob->color = prevBlob->color;
					drawRect(prevPrevBlob->boundingRect,attacksDisplayX,attacksDisplayY,prevPrevBlob->color,200);
					
					drawAttackText("  found matching blob to detect speed");
					
					int attackSpeed;
					bool onLeftSide = (prevBlobRect.x+prevBlobRect.width/2 < model->videoW/2);
					int player = (onLeftSide)?1:2;
					drawAttackText("  player: "+ofToString(player));
					if(onLeftSide)
						attackSpeed = (prevBlobRect.x+prevBlobRect.width)-(prevPrevBlobRect.x+prevPrevBlobRect.width);
					else 
						attackSpeed = (prevPrevBlobRect.x-prevBlobRect.x);
					
					drawAttackText("  attackSpeed: "+ofToString(attackSpeed));
					
					if(attackSpeed >= minAttackSpeed)
					{
						drawHitText("  Is attack");
						
						// analyze who's the victim
						
						int victim = (onLeftSide)? 2 : 1;
						drawAttackText("  victim: "+ofToString(victim));
						
						Blob * victimBlob = prevBlobs->at(victim-1);
						ofRectangle victimBlobRect = victimBlob->boundingRect;
						
						hitBlob.draw(attacksDisplayX,attacksDisplayY);
						
						if(rectHitTest(hitBlobRect,victimBlobRect))
						{
							if(model->debugDetection)
								drawRect(victimBlobRect, attacksDisplayX,attacksDisplayY, 0xffff00);	
							drawAttackText("  Hit!");
							drawHitText("  Hit hits a player");
							
							model->hitRect->x = hitBlobRect.x;
							model->hitRect->y = hitBlobRect.y;
							model->hitRect->width = hitBlobRect.width;
							model->hitRect->height = hitBlobRect.height;	
							
							model->hit(0,0,victim);
							
							drawHitText("  healths: "+ofToString(model->player1Health)+" vs. "+ofToString(model->player2Health));
							takeHitScreenShot("("+ofToString(model->hitCounter)+")");
						}
						else
						{
							drawAttackText("  no hit");
							takeHitScreenShot("("+ofToString(model->hitCounter)+")");
						}
					}
					else 
					{
						drawHitText("  Is not attack");
						takeHitScreenShot("("+ofToString(model->hitCounter)+")");
					}

				}
				else
				{
					drawAttackText("  not a matching blob to detect speed");	
				}
			}
			drawRect(prevBlob->boundingRect,attacksDisplayX,attacksDisplayY,prevBlob->color,255);
		}	
		model->possibleAttacksCounter++;
	}
}
bool VideoInputController::hitIsUnique(ofRectangle blobRect)
{
	//cout << "VideoInputController::hitIsUnique\n";
	bool toCloseToPrevHit = false;
	//cout << "  model->prevHitBlobs->size(): " << model->prevHitBlobs->size() << "\n";
	//cout << "  model->minDiffHitBlobsPos: " << model->minDiffHitBlobsPos << "\n";
	
	int numPrevHitBlobs = model->prevHitBlobs->size();
	for (int j = 0; j < numPrevHitBlobs; j++)
	{
		ofxCvBlob* prevBlob = model->prevHitBlobs->at(j);
		ofRectangle prevBlobRect = prevBlob->boundingRect;
		
		float dist = ofDist(prevBlobRect.x, prevBlobRect.y, 
							blobRect.x, blobRect.y);
		
		//cout << "  dist: " << dist << "\n";
		
		// if hit blobs are to close to a prev hit blob they are not unique
		if(dist < model->minDiffHitBlobsPos) 
			toCloseToPrevHit = true;
	}
	//cout << "  toCloseToPrevHit: " << toCloseToPrevHit << "\n";
	
	return !toCloseToPrevHit;
}
bool VideoInputController::hitsBody(ofRectangle hitBlobRect)
{
	//cout << "VideoInputController::hitsBody\n";
	int videoW = model->videoW;
	int videoH = model->videoH;
	int minNumWhitePixels = videoH/4;
	
	int hitX = hitBlobRect.x+hitBlobRect.width/2;
	
	int numWhitePixels = 0;
	unsigned char * pixels = model->grayDiffImg->getPixels();
	for (int i = 0; i < videoH; i++) 
	{
		int pixelIndex = (i * videoW) + hitX;
		if(pixels[pixelIndex] == 255)
			numWhitePixels++;
		else
			pixels[pixelIndex] = 200;
	}
	model->grayDiffImg->setFromPixels(pixels, videoW, videoH);
	if(model->debugDetection)
	{
		ofSetColor(0xffffff);
		model->grayDiffImg->draw(model->videoW, model->videoH);
	}
	
	return (numWhitePixels >= minNumWhitePixels);
}
bool VideoInputController::isCorrectColor(ofRectangle hitBlobRect, unsigned char * colorPixels)
{
	int videoW = model->videoW;
	int videoH = model->videoH;
	
	unsigned char * diffPixels = model->grayHitDiffImg->getPixels();
	
	int totalR = 0;
	int totalG = 0;
	int totalB = 0;
	
	ofxCvColorImage colorImg;
	colorImg.allocate(hitBlobRect.width, hitBlobRect.height);
	//unsigned char * newPixels = new unsigned char[videoW*videoH*3];
	unsigned char * newPixels = colorImg.getPixels();
	int newPixelIndex = 0;
	for (int i = hitBlobRect.y; i < hitBlobRect.y+hitBlobRect.height; i++) 
	{
		for(int j = hitBlobRect.x; j < hitBlobRect.x+hitBlobRect.width; j++) 
		{
			int pixelsIndex = (i * videoW) + j;
			
			if(diffPixels[pixelsIndex] == 255)
			{
				pixelsIndex *= 3; // color
				int r = colorPixels[pixelsIndex];
				totalR += r;
				int g = colorPixels[pixelsIndex+1];
				totalG += g;
				int b = colorPixels[pixelsIndex+2];
				totalB += b;
				
				newPixels[newPixelIndex] = r;
				newPixelIndex++;
				newPixels[newPixelIndex+1] = b;
				newPixelIndex++;
				newPixels[newPixelIndex+2] = g;
				newPixelIndex++;
			}
			else
			{
				pixelsIndex *= 3; // color
				if(model->debugDetection)
				{
					newPixels[newPixelIndex] = 0;
					newPixelIndex++;
					newPixels[newPixelIndex] = 0;
					newPixelIndex++;
					newPixels[newPixelIndex] = 0;
					newPixelIndex++;
				}
			}
			
		}
	}
	if(model->debugDetection)
	{
		ofSetColor(255, 255, 255);
		colorImg.setFromPixels(newPixels, hitBlobRect.width, hitBlobRect.height);
		colorImg.draw(videoW+hitBlobRect.x,hitBlobRect.y);
		
		
		int x = videoW+hitBlobRect.x;
		int y = hitBlobRect.y+hitBlobRect.height;
		int w = 5;
		int h;
		int spacing = 1;
		int numPixels = hitBlobRect.width*hitBlobRect.height;
		//r
		int r = totalR/numPixels;
		h = float(r)/255*20;
		ofSetColor(255, 0, 0);
		ofRect(x,y-h,w,h);
		//g
		int g = totalG/numPixels;
		h = float(g)/255*20;
		ofSetColor(0, 255, 0);
		ofRect(x+w+spacing,y-h,w,h);
		//b
		int b = totalB/numPixels;
		h = float(b)/255*20;
		ofSetColor(0, 0, 255);
		ofRect(x+w*2+spacing*2,y-h,w,h);
		
		
		
	}
	return true;
}
void VideoInputController::drawHitText(string text)
{
	if(!model->debugDetection) return;
	ofSetColor(0x0000ff);
	font.drawString(text, model->hitsTextX, model->hitsTextY+=model->lineHeight);
}
void VideoInputController::drawAttackText(string text)
{
	if(!model->debugDetection) return;
	ofSetColor(0x0000ff);
	font.drawString(text, model->attacksTextX, model->attacksTextY+=model->lineHeight);
}

void VideoInputController::analyzeShadowsForPlayers()
{
	bool detectedPlayer1 = false;
	bool detectedPlayer2 = false;

	int videoW = model->videoW;
	int videoH = model->videoH;
	ofRectangle * detectionZone = &model->detectionZone;
	int centerX = model->centerX;
	
    for (int i = 0; i < contourFinder.blobs.size(); i++)
	{
		ofxCvBlob blob = contourFinder.blobs[i];
		ofRectangle blobRect = blob.boundingRect;
		
		int blobRectCX = blobRect.x+blobRect.width/2;
		int blobRectCY = blobRect.y+blobRect.height/2;
		
		/*if(!(blobRect.x >= detectionZone->x &&
			 blobRect.y >= detectionZone->y &&
			 blobRect.x+blobRect.width <= detectionZone->x+detectionZone->width &&
			 blobRect.y+blobRect.height <= detectionZone->y+detectionZone->height))
			continue;*/
		
		if(!(blobRectCX >= detectionZone->x &&
			 blobRectCY >= detectionZone->y &&
			 blobRectCX <= detectionZone->x+detectionZone->width &&
			 blobRectCY <= detectionZone->y+detectionZone->height))
			continue;
		
		if(blobRectCX < model->centerX)
		{
			detectedPlayer1 = true;
		}
		else 
		{
			detectedPlayer2 = true;
		}
	}
	model->setDetectedPlayer1(detectedPlayer1);
	model->setDetectedPlayer2(detectedPlayer2);
	
}

// -- utils --
unsigned char * VideoInputController::toGrayscale(unsigned char * pixels)
{
	//cout << "VideoInputController::toGrayscale\n";
	int videoW = model->videoW;
	int videoH = model->videoH;
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
	return grayPixels;
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
		   y+rect.y, 		   rect.width, 
		   rect.height);
	ofSetColor(0xffffff);
	ofDisableAlphaBlending();

}
void VideoInputController::drawRect(ofRectangle rect,int x, int y, int color)
{
	drawRect(rect, x, y, color, 255);
}
void VideoInputController::takeScreenShot(string extraText)
{
	if(!model->takeScreenShots) return;
	string frameCounter = ofToString(model->frameCounter);
	ofSaveScreen("screenshots/frame"+frameCounter+" "+extraText+".png");
}
void VideoInputController::takeHitScreenShot(string extraText)
{
	if(!model->takeHitScreenShots) return;
	string frameCounter = ofToString(model->frameCounter);
	ofSaveScreen("screenshots/frame"+frameCounter+" (hit) "+extraText+".png");
}
bool VideoInputController::matchBlobs(ofxCvBlob * blob1, ofxCvBlob * blob2)
{
	//cout << "VideoInputController::matchBlobs\n";
	ofRectangle blobRect1 = blob1->boundingRect;
	ofRectangle blobRect2 = blob2->boundingRect;
	
	/*int blob1CX = blobRect1.x+blobRect1.width/2;
	int blob1CY = blobRect1.y+blobRect1.height/2;
	int blob2CX = blobRect2.x+blobRect2.width/2;
	int blob2CY = blobRect2.y+blobRect2.height/2;
	
	int dis = ofDist(blob1CX,blob1CY,blob2CX,blob2CY);*/
	
	int blob1CX = blobRect1.x+blobRect1.width/2;
	int blob2CX = blobRect2.x+blobRect2.width/2;
	int dis = blob2CX-blob1CX;
	
	if(dis < 0) dis *= -1;
	
	bool match = (dis < model->blobDiffTolerance);
	return match;
}
bool VideoInputController::rectHitTest(ofRectangle rect1,ofRectangle rect2)
{
	if(rect1.y					<	rect2.y+rect2.height &&
	   rect1.y+rect1.height		>	rect2.y &&
	   rect1.x					<	rect2.x+rect2.width &&
	   rect1.x+rect1.width		>	rect2.x)
		return true;
	else
		return false;
}
void VideoInputController::colorInImage(ofxCvGrayscaleImage * image, int color, ofRectangle rect)
{
	int width = image->getWidth();
	int height = image->getHeight();
	unsigned char * orgPixels = image->getPixels();
	//int newPixelsIndex = 0;
	for (int i = rect.y; i < rect.y+rect.height; i++) 
	{
		for(int j = rect.x; j < rect.x+rect.width; j++) 
		{
			int orgPixelsIndex = (i * width) + j;				
			//orgPixels[orgPixelsIndex] = newPixels[newPixelsIndex];
			orgPixels[orgPixelsIndex] = color;
		}
	}
	image->setFromPixels(orgPixels, width, height);
	
	//delete [] orgPixels;
}