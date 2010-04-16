/*
 *  Model.cpp
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 4/27/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "Model.h"s

Model::Model()
{
	cout << "Model::Model\n";
	videoW				= 640;
	videoH				= 460;
	threshold			= 35;
	hitThreshold		= 42; // is added to normal threshold
	willLearnBackground = false;
	minBlobSize			= 150*150;
	maxBlobSize			= videoH*videoH;
	maxNumBlobs			= 5;
	cameraIndex			= 0;
	debugDetection		= true;
	grayImg = new ofxCvGrayscaleImage();
	grayEmptyImg = new ofxCvGrayscaleImage();
	grayDiffImg = new ofxCvGrayscaleImage();
	grayImg->allocate(videoW,videoH);
	grayEmptyImg->allocate(videoW,videoH);
	grayDiffImg->allocate(videoW,videoH);
	
	blobs				= new vector<ofxCvBlob*>;
	prevGrayDiffImg		= new ofxCvGrayscaleImage();
	prevGrayDiffImg->allocate(videoW, videoH);
	
	hitting				= false;
	hitCounter			= 0;
	hitRect				= new ofRectangle();
	
	// load image with ofImage, store on ofxCvGrayscaleImage
	imgLoader = new ofImage();
	imgLoader->loadImage("empty.png");
	grayEmptyImg->setFromPixels(imgLoader->getPixels(), videoW,videoH);
	
	int emptyArg = 0;
	ofNotifyEvent(VALUES_UPDATED,emptyArg,this); 
	
	ofAddListener(ofEvents.update, this, &Model::update);
	ofAddListener(timer.TICK, this, &Model::onTick);
	
}

void Model::learnBackground()
{
	cout << "Model::learnBackground\n";
	willLearnBackground = true;
}
void Model::setThreshold(int newValue)
{
	cout << "Model::setThreshold:" << newValue << "\n";
	threshold = newValue;
	int emptyArg = 0;
	ofNotifyEvent(VALUES_UPDATED,emptyArg,this); 
}
void Model::setHitThreshold(int newValue)
{
	cout << "Model::setHitThreshold:" << newValue << "\n";
	hitThreshold = newValue;
	int emptyArg = 0;
	ofNotifyEvent(VALUES_UPDATED,emptyArg,this); 
}
void Model::setCameraIndex(int newValue)
{
	cout << "Model::setCameraIndex:" << newValue << "\n";
	cameraIndex = newValue;
	int emptyArg = 0;
	ofNotifyEvent(CAMERA_INDEX_CHANGED,emptyArg,this); 
}
void Model::hit()
{
	int emptyArg = 0;
	ofNotifyEvent(HIT,emptyArg,this); 
}

void Model::update(ofEventArgs & args)
{ 
	
}
void Model::onTick(int  & count)
{
	
}
