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
int Model::CLIP5_DEMO = 2;
int Model::CLIP6_DEMO = 3;

Model::Model()
{
	cout << "Model::Model\n";
	pixelsSource			= CLIP1_DEMO;
	videoW					= 640;
	videoH					= 480;
	
	blobDiffTolerance		= 75;
	maxBlobsHistoryLength	= 5;
	minDiffHitBlobsPos		= 50; //50;
	minAttackSpeed			= 0;//5;//15;
	state					= STATE_DEMO;
	willLearnBackground		= false;
	maxNumBlobs				= 5;
	cameraIndex				= 0;
	clip6EmptyCorrection	= 70;
	movieURL				= "";
	
	simpleHitBlobAnalysis	= true;
	centerX					= videoW/2;
	
	detectedPlayer1			= false;
	detectedPlayer2			= false;
	
	grayImg					= new ofxCvGrayscaleImage();
	grayEmptyImg			= new ofxCvGrayscaleImage();
	grayEmptyCopyImg		= new ofxCvGrayscaleImage();
	grayDiffImg				= new ofxCvGrayscaleImage();
	grayHitDiffImg			= new ofxCvGrayscaleImage();
	grayImg->allocate(videoW,videoH);
	grayEmptyImg->allocate(videoW,videoH);
	grayEmptyCopyImg->allocate(videoW,videoH);
	grayDiffImg->allocate(videoW,videoH);
	grayHitDiffImg->allocate(videoW,videoH);
	
	prevHitBlobs			= new vector<ofxCvBlob*>;
	currentBlobs			= new vector<Blob*>;
	blobsHistory			= new vector< vector<Blob*>* >;
	prevGrayDiffImg			= new ofxCvGrayscaleImage();
	prevGrayDiffImg->allocate(videoW, videoH);
		
	hitRect					= new ofRectangle();
	
	countDownTimer.setInterval(700);
	countDownTimer.setRepeatCount(5);
	ofAddListener(countDownTimer.TICK,this,&Model::onCountDownTick);
	
	finishTimer.setInterval(5000);
	finishTimer.setRepeatCount(1);
	ofAddListener(finishTimer.TICK,this,&Model::onFinishTick);
	
	// game logic
	gamePaused				= false;
	startHealth				= 100;
	hitDamage				= 3; //3; //7;//3;
	
	// debugging
	debug					= true;
	debugDetection			= true;
	slowMotion				= false;
	takeScreenShots			= false;
	takeHitScreenShots		= false;
	videoPaused				= false;
	hitsTextY				= 0;
	attacksTextY			= 0;
	lineHeight				= 20;
	
	resetGame();
	
	if(slowMotion)
		ofSetFrameRate(3);
	
	cout << "Model:: calling setup \n";
	soundController.setup();
	cout << "Model:: called setup \n";
}
void Model::resetGame()
{
	player1Health			= startHealth;
	player2Health			= startHealth;	
	winner					= 0;
	
	// debugging
	possibleAttacksCounter	= 0;
	hitCounter				= -1;
	frameCounter			= -1;
	
	int emptyArg = 0;
	ofNotifyEvent(RESET,emptyArg,this); 
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
	
	/*unsigned char * pixels = imgLoader->getPixels();
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
	
	ofImage imgSaver = *new ofImage();
	imgSaver.setFromPixels(grayPixels, videoW, videoH, OF_IMAGE_GRAYSCALE, false);
	imgSaver.saveImage("empty clip6.png");*/
	
	grayEmptyImg->setFromPixels(imgLoader->getPixels(), videoW,videoH);
	grayEmptyCopyImg->setFromPixels(imgLoader->getPixels(), videoW,videoH);
	
	delete imgLoader;
	
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
		
		//minBlobArea = 10;
	}
	else if(pixelsSource == CLIP5_DEMO)
	{
		threshold = 35;
		hitThreshold = 40;
		backgroundImageURL = "empty clip5.png";
		movieURL = "movies/clip5.mov";
		
		hitDetectionZone.x = 200;
		hitDetectionZone.width = videoW-hitDetectionZone.x*2;
		
		//hitThreshold = 75;
		//minBlobArea = 5;
	}
	else if(pixelsSource == CLIP6_DEMO)
	{
		threshold = 37;
		hitThreshold = 64;
		backgroundImageURL = "empty clip6.png";
		movieURL = "movies/clip6";
		//movieURL = "movies/clip1.mov";
		detectionZone.x = hitDetectionZone.x = 75;
		centerX = detectionZone.x+detectionZone.width/2;
	}
	else 
	{
		backgroundImageURL = "empty.png";
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
	if(pixelsSource == CAMERA)
	{
		xml.setValue("backgroundImage", backgroundImageURL);
		xml.setValue("threshold", threshold);
		xml.setValue("hitThreshold", hitThreshold);
		
		xml.setValue("detectionZoneX", detectionZone.x);
		xml.setValue("detectionZoneY", detectionZone.y);
		xml.setValue("detectionZoneWidth", detectionZone.width);
		xml.setValue("detectionZoneHeight", detectionZone.height);
		xml.setValue("hitDetectionZoneX", hitDetectionZone.x);
		xml.setValue("hitDetectionZoneY", hitDetectionZone.y);
		xml.setValue("hitDetectionZoneWidth", hitDetectionZone.width);
		xml.setValue("hitDetectionZoneHeight", hitDetectionZone.height);
	}
	xml.setValue("minBlobArea", minBlobArea);
	xml.setValue("maxBlobArea", maxBlobArea);
	xml.setValue("minHitBlobArea", minHitBlobArea);
	xml.setValue("maxHitBlobArea", maxHitBlobArea);
	xml.setValue("maxNumBlobs", maxNumBlobs);
	xml.setValue("maxNumHitBlobs", maxNumHitBlobs);
	xml.setValue("minDiffHitBlobsPos", minDiffHitBlobsPos);
	xml.setValue("debugDetection", debugDetection);
	
	
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
void Model::hit(int type, int area, int victim)
{
	cout << "Model::hit:\n";
	if(gamePaused) return;
	
	soundController.PlaySound(victim);
	
	if(victim == 1)
		player1Health -= hitDamage;
	else 
		player2Health -= hitDamage;
	
	if(player1Health < 0) player1Health = 0;
	if(player2Health < 0) player2Health = 0;
	
	cout << "  "<<player1Health<<" : "<<player2Health<<"\n";
	
	int emptyArg = 0;
	ofNotifyEvent(HIT,emptyArg,this); 
	
	if(player1Health <= 0 || player2Health <= 0)
	{
		winner = (player1Health <= 0)? 2 : 1;
		setState(STATE_GAME_FINISHED);
	}
}
void Model::setState(int newValue)
{
	cout << "Model::setState:" << newValue << "\n";
	state = newValue;
	
	switch(state)
	{
		case STATE_COUNT_DOWN:
			countDown = countDownTimer.getRepeatCount();
			countDownTimer.reset();
			countDownTimer.start();
			break;
		case STATE_GAME:
			if(!gamePaused)
				resetGame();
			gamePaused = false;
			break;
		case STATE_WAITING:
			gamePaused = true;
			break;
		case STATE_GAME_FINISHED:
			finishTimer.start();
			detectedPlayer1 = false;
			detectedPlayer2 = false;
			break;
	}
	
	int emptyArg = 0;
	ofNotifyEvent(STATE_CHANGE,emptyArg,this);
}
void Model::setVideoPause(bool newValue)
{
	cout << "Model::setVideoPause:" << newValue << "\n";
	videoPaused = newValue;
	
	int emptyArg = 0;
	if(videoPaused)
		ofNotifyEvent(VIDEO_PAUSE,emptyArg,this);
	else
		ofNotifyEvent(VIDEO_RESUME,emptyArg,this);
}
void Model::setDetectedPlayer1(bool newValue)
{
	if(newValue == detectedPlayer1) return;
	detectedPlayer1 = newValue;
	
	int emptyArg = 0;
	ofNotifyEvent(PLAYERS_CHANGED,emptyArg,this);
	
	checkPlayers();
}
void Model::setDetectedPlayer2(bool newValue)
{
	if(newValue == detectedPlayer2) return;
	detectedPlayer2 = newValue;
	
	int emptyArg = 0;
	ofNotifyEvent(PLAYERS_CHANGED,emptyArg,this);
	
	checkPlayers();
}

void Model::checkPlayers()
{
	cout << "Model::checkPlayers\n";
	cout << "  state: "<<state<<"\n";
	cout << "  detectedPlayer1: "<<detectedPlayer1<<"\n";
	cout << "  detectedPlayer2: "<<detectedPlayer2<<"\n";
	if(state == STATE_DEMO && detectedPlayer1 && detectedPlayer2)
	{
		setState(STATE_COUNT_DOWN);
	}
	else if(state == STATE_GAME && !detectedPlayer1 && !detectedPlayer2)
	{
		//setState(STATE_WAITING);
	}
	else if(state == STATE_WAITING && detectedPlayer1 && detectedPlayer2)
	{
		setState(STATE_GAME);
	}
	else if(state != STATE_GAME  && state != STATE_GAME_FINISHED  && (!detectedPlayer1 || !detectedPlayer2))
	{
		setState(STATE_DEMO);
		countDownTimer.reset();
	}
}

void Model::update(ofEventArgs & args)
{ 
	
}
void Model::onCountDownTick(int  & count)
{
	cout << "Model::onCountDownTick\n";
	cout << "  count: "<<count<<"\n";
	
	if(count < countDownTimer.getRepeatCount()-1)
	{
		
		countDown = countDownTimer.getRepeatCount()-2-count;
		cout << "  countdown: "<<countDown<<"\n";
		int emptyArg = 0;
		ofNotifyEvent(COUNT_DOWN,emptyArg,this);
	}
	else 
	{
		cout << "  start game\n";
		setState(STATE_GAME);
	}
}
void Model::onFinishTick(int  & count)
{
	cout << "Model::onFinishTick\n";
	cout << "  count: "<<count<<"\n";
	
	setState(STATE_DEMO);
}