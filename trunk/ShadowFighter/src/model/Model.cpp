/*
 *  Model.cpp
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 4/27/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "Model.h"

int Model::CAMERA = 0;
int Model::CLIP1_DEMO = 1;

Model::Model()
{
	cout << "Model::Model\n";
	
	pixelsSource		= CAMERA;
	
	videoW				= 640;
	videoH				= 480;
	willLearnBackground = false;
	maxNumBlobs			= 5;
	cameraIndex			= 0;
	debugDetection		= true;
	minDiffHitBlobsPos	= 50;
	movieURL			= "";
	
	grayImg = new ofxCvGrayscaleImage();
	grayEmptyImg = new ofxCvGrayscaleImage();
	grayDiffImg = new ofxCvGrayscaleImage();
	grayImg->allocate(videoW,videoH);
	grayEmptyImg->allocate(videoW,videoH);
	grayDiffImg->allocate(videoW,videoH);
	
	blobs				= new vector<ofxCvBlob*>;
	prevHitBlobs		= new vector<ofxCvBlob*>;
	prevGrayDiffImg		= new ofxCvGrayscaleImage();
	prevGrayDiffImg->allocate(videoW, videoH);
	
	hitting				= false;
	hitCounter			= 0;
	hitRect				= new ofRectangle();
	
}
void Model::loadData()
{
	cout << "Model::loadData\n";
	if(xml.loadFile("config.xml"))
	{
		cout << "config.xml loaded\n";
	}
	else
	{
		cout << "Unable to load config.xml check data folder\n";
		return;
	}
	parseXML();
	storeValues();
	
	// load image with ofImage, store on ofxCvGrayscaleImage
	imgLoader = new ofImage();
	imgLoader->loadImage(backgroundImageURL);
	grayEmptyImg->setFromPixels(imgLoader->getPixels(), videoW,videoH);
	
	int emptyArg = 0;
	ofNotifyEvent(DATA_LOADED,emptyArg,this);
	ofNotifyEvent(VALUES_UPDATED,emptyArg,this);
}

void Model::parseXML()
{
	cout << "Model::parseXML\n";
	backgroundImageURL		= xml.getValue("backgroundImage", "url");
	threshold				= xml.getValue("threshold", 1);
	hitThreshold			= xml.getValue("hitThreshold", 1);	
	minBlobArea				= xml.getValue("minBlobArea", 1);	
	maxBlobArea				= xml.getValue("maxBlobArea", 1);	
	minHitBlobArea			= xml.getValue("minHitBlobArea", 1);	
	maxHitBlobArea			= xml.getValue("maxHitBlobArea", 1);	
	maxNumBlobs				= xml.getValue("maxNumBlobs", 1);
	maxNumHitBlobs			= xml.getValue("maxNumHitBlobs", 1);
	minDiffHitBlobsPos		= xml.getValue("minDiffHitBlobsPos", 1);
	debugDetection			= xml.getValue("debugDetection", 1);

	detectionZone.x			=  xml.getValue("detectionZoneX", 1);	
	detectionZone.y			=  xml.getValue("detectionZoneY", 1);	
	detectionZone.width		=  xml.getValue("detectionZoneWidth", 1);	
	detectionZone.height	=  xml.getValue("detectionZoneHeight", 1);	
	
	hitDetectionZone.x		=  xml.getValue("hitDetectionZoneX", 1);	
	hitDetectionZone.y		=  xml.getValue("hitDetectionZoneY", 1);	
	hitDetectionZone.width	=  xml.getValue("hitDetectionZoneWidth", 1);	
	hitDetectionZone.height	=  xml.getValue("hitDetectionZoneHeight", 1);	
	
	if(pixelsSource == CLIP1_DEMO)
	{
		threshold = 35;
		hitThreshold = 47;
		backgroundImageURL = "empty clip1.png";
		movieURL = "movies/clip1.mov";
	}
	
	cout << "backgroundImageURL: " << backgroundImageURL << "\n";
	cout << "threshold: " << threshold << "\n";
	cout << "hitThreshold: " << hitThreshold << "\n";
	cout << "minBlobArea: " << minBlobArea << "\n";
	cout << "maxBlobArea: " << maxBlobArea << "\n";
	cout << "minHitBlobArea: " << minHitBlobArea << "\n";
	cout << "maxHitBlobArea: " << maxHitBlobArea << "\n";
	cout << "maxNumBlobs: " << maxNumBlobs << "\n";
	cout << "maxNumHitBlobs: " << maxNumHitBlobs << "\n";
	cout << "minDiffHitBlobsPos: " << minDiffHitBlobsPos << "\n";
	cout << "debugDetection: " << debugDetection << "\n";
	cout << "detectionZone.x: " << detectionZone.x << "\n";
	cout << "detectionZone.y: " << detectionZone.y << "\n";
	cout << "detectionZone.width: " << detectionZone.width << "\n";
	cout << "detectionZone.height: " << detectionZone.height << "\n";
	cout << "hitDetectionZone.x: " << hitDetectionZone.x << "\n";
	cout << "hitDetectionZone.y: " << hitDetectionZone.y << "\n";
	cout << "hitDetectionZone.width: " << hitDetectionZone.width << "\n";
	cout << "hitDetectionZone.height: " << hitDetectionZone.height << "\n";
}
void Model::storeValues()
{	
	cout << "Model::storeValues\n";
	//if(demoMovie != NO_DEMO) return;
	
	xml.setValue("backgroundImage", backgroundImageURL);
	xml.setValue("threshold", threshold);
	xml.setValue("hitThreshold", hitThreshold);
	xml.setValue("minBlobArea", minBlobArea);
	xml.setValue("maxBlobArea", maxBlobArea);
	xml.setValue("minHitBlobArea", minHitBlobArea);
	xml.setValue("maxHitBlobArea", maxHitBlobArea);
	xml.setValue("maxNumBlobs", maxNumBlobs);
	xml.setValue("maxNumHitBlobs", maxNumHitBlobs);
	xml.setValue("minDiffHitBlobsPos", minDiffHitBlobsPos);
	xml.setValue("debugDetection", debugDetection);
	xml.setValue("detectionZoneX", detectionZone.x);
	xml.setValue("detectionZoneY", detectionZone.y);
	xml.setValue("detectionZoneWidth", detectionZone.width);
	xml.setValue("detectionZoneHeight", detectionZone.height);
	xml.setValue("hitDetectionZoneX", hitDetectionZone.x);
	xml.setValue("hitDetectionZoneY", hitDetectionZone.y);
	xml.setValue("hitDetectionZoneWidth", hitDetectionZone.width);
	xml.setValue("hitDetectionZoneHeight", hitDetectionZone.height);
	
	xml.saveFile("config.xml");
}
void Model::start()
{
	cout << "Model::start\n";
	ofAddListener(ofEvents.update, this, &Model::update);
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
	storeValues();
}
void Model::setHitThreshold(int newValue)
{
	cout << "Model::setHitThreshold:" << newValue << "\n";
	hitThreshold = newValue;
	int emptyArg = 0;
	ofNotifyEvent(VALUES_UPDATED,emptyArg,this); 
	storeValues();
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
