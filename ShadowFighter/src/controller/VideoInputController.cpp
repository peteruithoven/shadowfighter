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
	font.loadFont("mono.ttf", 10);
}
bool sortBlobsOnX(ofxCvBlob * blob1, ofxCvBlob * blob2)
{
	return (blob1->boundingRect.x < blob2->boundingRect.x); 
}
void VideoInputController::newPixels(unsigned char * pixels)
{
	analyze(pixels);
}
void VideoInputController::analyze(unsigned char * pixels)
{
	cout << "VideoInputController::analyze\n";
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
	if(model->debugDetection)
		model->grayEmptyImg->draw(videoW, 0);
	delete [] grayPixels;
	// filters
	filterProjection();
	// take the abs value of the difference between background and incoming and then threshold:
	model->grayDiffImg->absDiff(*model->grayEmptyImg, *model->grayImg);
	model->grayDiffImg->threshold(model->threshold);
	if(model->debugDetection)
		model->grayDiffImg->draw(videoW, videoH);
	// find normal blobs
	contourFinder.findContours(*model->grayDiffImg, model->minBlobArea, model->maxBlobArea, model->maxNumBlobs, true);
	
	
	int numBlobs = contourFinder.blobs.size();
	//int numPrevBlobs = model->blobs->size();
	
	if(model->debugDetection)
		drawBlobsHistory();
	
	//// hit blobs
	findHitBlobs();
	
	if(hitContourFinder.blobs.size() > 0)
		analyseHitBlobs();
	
	storeHistory();
	
	takeScreenShot("");
}
void VideoInputController::storeBackgroundImage(unsigned char * grayPixels)
{
	cout << "VideoInputController::storeBackgroundImage\n";
	model->grayEmptyImg->setFromPixels(grayPixels,model->videoW,model->videoH);
	
	ofImage imgSaver = *new ofImage();
	imgSaver.setFromPixels(grayPixels, model->videoW, model->videoH, OF_IMAGE_GRAYSCALE, false);
	imgSaver.saveImage("empty.png");
}
void VideoInputController::filterProjection()
{
	cout << "VideoInputController::filterProjection\n";
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
			if(pixels[pixelIndex] > 180)
			{
				pixels[pixelIndex] = backgroundPixels[pixelIndex];
			}
		}
	}
	model->grayImg->setFromPixels(pixels, videoW, videoH);
	
	if(model->debugDetection)
		model->grayImg->draw(0,0);
}
void VideoInputController::findHitBlobs()
{
	cout << "VideoInputController::findHitBlobs\n";
	int hitBlobDisplayX = 0;
	int hitBlobDisplayY = model->videoH;
	
	model->grayHitDiffImg->absDiff(*model->grayEmptyImg, *model->grayImg);
	//model->grayHitDiffImg->draw(0, videoH);
	
	model->grayHitDiffImg->threshold(model->threshold+model->hitThreshold);
	if(model->debugDetection)
		model->grayHitDiffImg->draw(hitBlobDisplayX, hitBlobDisplayY);
	
	ofRectangle * detectionZone = &model->hitDetectionZone;
	if(model->debugDetection)
	{
		ofNoFill();
		ofSetColor(0xcc0000);
		ofRect(detectionZone->x, detectionZone->y, detectionZone->width, detectionZone->height);
		ofRect(hitBlobDisplayX+detectionZone->x, hitBlobDisplayY+detectionZone->y, detectionZone->width, detectionZone->height);
	}
	
	hitContourFinder.findContours(*model->grayHitDiffImg, model->minHitBlobArea, model->maxHitBlobArea, model->maxNumHitBlobs, true);
}
void VideoInputController::storeHistory()
{
	cout << "VideoInputController::storeHistory\n";
	// store copy of hit blobs
	if(model->prevHitBlobs->size() > 0)
		model->prevHitBlobs->clear();
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
	
	// store and draw blobs
	vector<Blob*> * prevBlobs = new vector<Blob*>;
    for (int i = 0; i < contourFinder.blobs.size(); i++)
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

void VideoInputController::drawBlobsHistory()
{
	cout << "VideoInputController::drawBlobsHistory\n";
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
void VideoInputController::analyseHitBlobs()
{
	cout << "VideoInputController::analyseHitBlobs (frame: "<<model->frameCounter<<")\n";
	
	int hitBlobDisplayX = 0;
	int hitBlobDisplayY = model->videoH;
	int attacksDisplayX = model->videoW*2;
	int attacksDisplayY = model->videoH;
	
	model->hitsTextX = hitBlobDisplayX;
	model->hitsTextY = hitBlobDisplayY;
	
	
	drawHitText("  Found hit blobs ");
	
	// if there where not 2 blobs before this frame, we can't check for an attack, so cancel
	vector<vector<Blob*> * > * blobsHistory = model->blobsHistory;
	int historyLength = blobsHistory->size();
	if(blobsHistory->size() < 1) 
	{
		drawHitText("  There is not enough blob history to detect attacks");
		return;
	}
	drawHitText("  There is enough blob history to detect attacks");

	vector<Blob*> * prevBlobs = blobsHistory->at(blobsHistory->size()-1);
	if(prevBlobs->size() < 2)
	{
		drawHitText("  There weren't 2 blobs in prev blobs\n");
		return;
	}
	drawHitText("  There were 2 blobs in prev blobs\n");
	
	ofRectangle * detectionZone = &model->hitDetectionZone;
	
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
			takeScreenShot("hit ("+ofToString(model->hitCounter)+")");
			continue;
		}
		drawHitText("  Hit blob is a unique hit blob");
		
		cout << "VideoInputController::there was an attack? (" << model->possibleAttacksCounter << ")\n";
		
		// Analyze possible attack
		string counters = ofToString(model->hitCounter)+"/"+ofToString(model->possibleAttacksCounter);
		
		//int blobDiffTolerance = 100;
		int minAttackSpeed = model->minAttackSpeed;
		cout << "  minAttackSpeed: " << minAttackSpeed << "\n";
		bool isAttack = false;
		
		drawAttackText("there was an attack? ("+counters+")");
		
		ofSetColor(0xffffff);
		if(model->debugDetection)
			model->prevGrayDiffImg->draw(attacksDisplayX,attacksDisplayY);
		
		int historyLength = model->blobsHistory->size();
		if(historyLength < model->maxBlobsHistoryLength) 
		{
			drawAttackText("  not enough blobs history");
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
					
					//if(attackSpeed >= minAttackSpeed)
					//{
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
						}
						else
						{
							drawAttackText("  no hit");
						}
							
						takeScreenShot("hit ("+ofToString(model->hitCounter)+")");
					//}
					//else 
					//{
					//	drawHitText("  Is not attack");
					//}

				}
			}
			drawRect(prevBlob->boundingRect,attacksDisplayX,attacksDisplayY,prevBlob->color,255);
		}	
		model->possibleAttacksCounter++;
	}
}
bool VideoInputController::hitIsUnique(ofRectangle blobRect)
{
	cout << "VideoInputController::hitIsUnique\n";
	bool toCloseToPrevHit = false;
	cout << "  model->prevHitBlobs->size(): " << model->prevHitBlobs->size() << "\n";
	cout << "  model->minDiffHitBlobsPos: " << model->minDiffHitBlobsPos << "\n";
	
	int numPrevHitBlobs = model->prevHitBlobs->size();
	for (int j = 0; j < numPrevHitBlobs; j++)
	{
		ofxCvBlob* prevBlob = model->prevHitBlobs->at(j);
		ofRectangle prevBlobRect = prevBlob->boundingRect;
		
		float dist = ofDist(prevBlobRect.x, prevBlobRect.y, 
							blobRect.x, blobRect.y);
		
		cout << "  dist: " << dist << "\n";
		
		if(dist < model->minDiffHitBlobsPos) 
			toCloseToPrevHit = true;
	}
	cout << "  toCloseToPrevHit: " << toCloseToPrevHit << "\n";
	
	return !toCloseToPrevHit;
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

// -- utils --
unsigned char * VideoInputController::toGrayscale(unsigned char * pixels)
{
	cout << "VideoInputController::toGrayscale\n";
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
bool VideoInputController::matchBlobs(ofxCvBlob * blob1, ofxCvBlob * blob2)
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
