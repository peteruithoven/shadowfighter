/*
 *  Model.cpp
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 4/27/09.
 *
 */

#include "Model.h"

int Model::CAMERA = 0;
int Model::CLIP1_DEMO = 1;
int Model::CLIP5_DEMO = 2;
int Model::CLIP6_DEMO = 3;
int Model::BLOCK_FIGHT_DEMO = 4;
int Model::BLOCK_EXPERIMENTS_DEMO = 5;
int Model::FIGHT2_DEMO =  6;
int Model::FIGHT3_DEMO = 7;

Model::Model()
{
	cout << "Model::Model\n";
	pixelsSource			= CAMERA;
	videoW					= 640;
	videoH					= 480;
	
	blobDiffTolerance		= 75;
	maxBlobsHistoryLength	= 5;
	minDiffHitBlobsPos		= 75; //50;
	minAttackSpeed			= 0;//5;//15;
	state					= STATE_DEMO;
	willLearnBackground		= false;
	maxNumBlobs				= 5;
	cameraIndex				= 0;
	clip6EmptyCorrection	= 70;
	simpleHitBlobAnalysis	= true;
	centerX					= videoW/2;
	
	movieURL				= "";
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
	
	countDownTimer.setInterval(1200); //800//1500//1200//700
	countDownTimer.setRepeatCount(3); //5
	ofAddListener(countDownTimer.TICK,this,&Model::onCountDownTick);
	
	finishTimer.setInterval(8000); //5000
	finishTimer.setRepeatCount(1);
	ofAddListener(finishTimer.TICK,this,&Model::onFinishTick);
	
	waitingDelayer.setInterval(1000);
	waitingDelayer.setRepeatCount(1);
	ofAddListener(waitingDelayer.TICK,this,&Model::onWaitingDelayed);
	
	demoDelayer.setInterval(3000);
	demoDelayer.setRepeatCount(1);
	ofAddListener(demoDelayer.TICK,this,&Model::onDemoDelayed);
	
	// game logic
	gamePaused				= false;
	startHealth				= 200;
	//hitDamage				= 3; //7;//3;
	//powerHitDamage			= 10;
	HitTypeVO *hitTypeVO;
	hitTypeVO = new HitTypeVO(HIT_HEAD,0.2,8); //8
	hitTypes.push_back(hitTypeVO);
	hitTypeVO = new HitTypeVO(HIT_BODY,(0.2+0.47),5); //5
	hitTypes.push_back(hitTypeVO);
	hitTypeVO = new HitTypeVO(HIT_LEGS,2,3); //3
	hitTypes.push_back(hitTypeVO);
	powerHitTypeVO = new HitTypeVO(HIT_POWER,0,13); //13
	hitTypes.push_back(powerHitTypeVO);
	
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
	videoPosition			= 0;
	overulePlayer1Detected	= false;
	overulePlayer2Detected	= false;
	
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
	player1.body			= NULL;
	player1.mainBody		= NULL;
	player1.block			= NULL;
	player1.powerHit		= false;
	player2.body			= NULL;
	player2.mainBody		= NULL;
	player2.block			= NULL;
	player2.powerHit		= false;
	
	gamePaused				= true;
	
	// debugging
	possibleAttacksCounter	= 0;
	hitCounter				= -1;
	frameCounter			= -1;
	
	int emptyArg = 0;
	ofNotifyEvent(RESET,emptyArg,this); 
	
	int empty = 0;
	ofNotifyEvent(POWERHITS_UPDATE,empty,this); 
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
	
//	unsigned char * pixels = imgLoader->getPixels();
//	unsigned char * grayPixels = new unsigned char[videoW*videoH];
//	int counter = 0;
//	for (int i = 0; i < videoH; i++) {
//		for (int j = 0; j < videoW*3; j+=3) {
//			
//			// get r, g and b components
//			int r = (i*videoW*3) + j;
//			int g = (i*videoW*3) + (j+1);
//			int b = (i*videoW*3) + (j+2);
//			
//			int grayPixel = (11 * pixels[r] + 16 * pixels[g] + 5 * pixels[b]) / 32;
//			
//			grayPixels[counter] = grayPixel;
//			
//			counter++;
//		}
//	}
//	
//	ofImage imgSaver = *new ofImage();
//	imgSaver.setFromPixels(grayPixels, videoW, videoH, OF_IMAGE_GRAYSCALE, false);
//	imgSaver.saveImage(backgroundImageURL);
	
	grayEmptyImg->setFromPixels(imgLoader->getPixels(), videoW,videoH);
	grayEmptyImg->blur(2);
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
	
	player1.area = new ofRectangle();
	player2.area = new ofRectangle();
	player1.area->width = player2.area->width = 175;
	player1.area->height = player2.area->height = 330;
	player1.area->y = player2.area->y = 50;
	player1.area->x = centerX-player1.area->width;
	player2.area->x = centerX;
	
//	detectionZone.x			=  0;	
//	detectionZone.width		=  videoW;
	
	maxNumHitBlobs = 50;
	
	if(pixelsSource == CLIP1_DEMO)
	{
		threshold = 35;
		hitThreshold = 47;
		backgroundImageURL = "empty clip1.png";
		movieURL = "movies/clip1.mov";
		
		detectionZone.width = hitDetectionZone.width = 440;
		detectionZone.height= hitDetectionZone.height = 480;
		detectionZone.x		= centerX-detectionZone.width/2;
		hitDetectionZone.x	= centerX-hitDetectionZone.width/2;
		detectionZone.y		= hitDetectionZone.y = videoH/2-detectionZone.height/2;
		
		//minBlobArea = 10;
	}
	else if(pixelsSource == CLIP5_DEMO)
	{
		threshold = 35;
		hitThreshold = 40;
		backgroundImageURL = "empty clip5.png";
		movieURL = "movies/clip5.mov";
		
		detectionZone.width = hitDetectionZone.width = 500;
		detectionZone.height= hitDetectionZone.height = 480;
		detectionZone.x		= centerX-detectionZone.width/2;
		hitDetectionZone.x	= centerX-hitDetectionZone.width/2;
		detectionZone.y		= hitDetectionZone.y = videoH/2-detectionZone.height/2;
		
		player1.area->width = player2.area->width = detectionZone.width/2;
		player1.area->height = player2.area->height = detectionZone.height;
		player1.area->y = player2.area->y = videoH/2-player1.area->height/2;
		player1.area->x = centerX-player1.area->width;
		player2.area->x = centerX;
		
		videoPosition = 0.2;
		
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
		
		centerX = 300;
		detectionZone.width = hitDetectionZone.width = 500;
		detectionZone.height= hitDetectionZone.height = 430;
		detectionZone.x		= centerX-detectionZone.width/2;
		hitDetectionZone.x	= centerX-hitDetectionZone.width/2;
		detectionZone.y		= hitDetectionZone.y = 20;
		
	}
	else if(pixelsSource == BLOCK_FIGHT_DEMO)
	{
		threshold = 22;
		hitThreshold = 37;
		backgroundImageURL = "empty block-fight.png";
		movieURL = "movies/block-fight.mov";
		centerX = 270;
		detectionZone.x = centerX-detectionZone.width/2;
		hitDetectionZone.x = centerX-hitDetectionZone.width/2;
		detectionZone.y = hitDetectionZone.y = 70;
		//detectionZone.height = hitDetectionZone.height -= 10;
		
//		player1.area->width = player2.area->width = detectionZone.width/2;
//		player1.area->height = player2.area->height = detectionZone.height;
//		player1.area->y = player2.area->y = videoH/2-player1.area->height/2;
//		player1.area->x = centerX-player1.area->width;
//		player2.area->x = centerX;
	}
	else if(pixelsSource == BLOCK_EXPERIMENTS_DEMO)
	{
		threshold			= 22;
		hitThreshold		= 37;
		backgroundImageURL	= "empty block-experiments2.png";
		movieURL			= "movies/block-experiments.mov";
		centerX				= 270;
		detectionZone.x		= centerX-detectionZone.width/2;
		hitDetectionZone.x	= centerX-hitDetectionZone.width/2;
		detectionZone.y		= hitDetectionZone.y = 60;
		detectionZone.height= hitDetectionZone.height= 290+38;
		videoPosition		= 0.5; // ducking
	}
	else if(pixelsSource == FIGHT2_DEMO)
	{
		threshold			= 22;
		hitThreshold		= 37;
		backgroundImageURL	= "empty fight2.png";
		movieURL			= "movies/fight2.mov";
		centerX				= 270+53;
		detectionZone.x		= centerX-detectionZone.width/2;
		hitDetectionZone.x	= centerX-hitDetectionZone.width/2;
		detectionZone.y		= hitDetectionZone.y = 55;
		detectionZone.height= hitDetectionZone.height= 290+38;
		maxNumHitBlobs		= 10;
		//videoPosition		= 0.06; // just faster
		//videoPosition		= 0.12; // fight right away
		//videoPosition		= 0.53; // fight right away
		//setState(STATE_GAME);
	}
	else if(pixelsSource == FIGHT3_DEMO)
	{
		threshold			= 22;
		hitThreshold		= 42;
		backgroundImageURL	= "empty fight3.png";
		movieURL			= "movies/fight3.mov";
		centerX				= 270+53;
		detectionZone.x		= centerX-detectionZone.width/2;
		hitDetectionZone.x	= centerX-hitDetectionZone.width/2;
		detectionZone.y		= hitDetectionZone.y = 55;
		maxNumHitBlobs		= 10;
		setState(STATE_GAME);
	}
	else 
	{
		backgroundImageURL = "empty.png";
		//setState(STATE_GAME);
	}
	
	player1.area->width = player2.area->width = detectionZone.width/2;
	player1.area->height = player2.area->height = detectionZone.height;
	player1.area->y = player2.area->y = detectionZone.y; //player2.area->y = videoH/2-player1.area->height/2;
	player1.area->x = detectionZone.x; //centerX-player1.area->width;
	player2.area->x = detectionZone.x+detectionZone.width/2;
	
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
	if(pixelsSource != CAMERA) return;

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
void Model::hit(HitVO hitVO)
{
	cout << "Model::hit\n";
	if(gamePaused) return;
	
	cout << "  victim: " << hitVO.victim << "\n";
	
	PlayerVO *victim = (hitVO.victim == 1)? &player1 : &player2;
	PlayerVO *attacker = (hitVO.victim == 1)? &player2 : &player1;
	
	cout << "  attacker->powerHit: " << attacker->powerHit << "\n";
	
	//if(victim->powerHit)
	//	setVideoPause(true);
	int bodyPart = 2;
	soundController.PlayHitSound(SOUND_HIT,bodyPart,attacker->powerHit,hitVO.victim);
	
	//hitVO.type = int(ofRandom(0, 3));
	if(attacker->powerHit)
		hitVO.typeVO = powerHitTypeVO;
	
	cout << "  type: " << hitVO.typeVO->name << "\n";
	
	
	//int damage = (attacker->powerHit)? powerHitDamage : hitDamage;
	int damage = hitVO.typeVO->damage;
	cout << "  damage: " << damage << "\n";
	
	if(hitVO.victim == 1)
		player1Health -= damage;
	else 
		player2Health -= damage;
	
	if(player1Health < 0) player1Health = 0;
	if(player2Health < 0) player2Health = 0;
	
	cout << "  "<<player1Health<<" : "<<player2Health<<"\n";
	
	ofNotifyEvent(HIT,hitVO,this); 
	
	
	if(player1Health <= 0 || player2Health <= 0)
	{
		winner = (player1Health <= 0)? 2 : 1;
		setState(STATE_GAME_FINISHED);
	}
	attacker->powerHit = false;
	
	int empty = 0;
	ofNotifyEvent(POWERHITS_UPDATE,empty,this); 
}
void Model::block(BlockVO *blockVO)
{
	cout << "Model::block\n";
	if(gamePaused) return;
	cout << " victim: " << blockVO->victim << "\n";
	//setVideoPause(true);
	
	PlayerVO *victim = (blockVO->victim == 1)? &player1 : &player2;
	bool hadPowerHit = victim->powerHit;
	victim->powerHit = true;
	
	soundController.PlayBlockSound(SOUND_BLOCK,blockVO->victim,!hadPowerHit);
	
	ofNotifyEvent(BLOCK,*blockVO,this); 
	
	if(hadPowerHit != victim->powerHit)
	{
		int empty = 0;
		ofNotifyEvent(POWERHITS_UPDATE,empty,this); 
	}
}
void Model::checkBlocks()
{
	//cout << "Model::checkBlocks:\n";
	if(gamePaused) return;
	
//	for(int i=0;i<blockVOs.size();i++)
//	{
//		BlockVO *blockVO = blockVOs.at(i);
//		//blockVO->mirrorX = (blockVO->bounds.x > centerX);
//		ofNotifyEvent(BLOCKING,*blockVO,this); 
//	}
	if(player1.blockVO != NULL)
		ofNotifyEvent(BLOCKING,*player1.blockVO,this); 
	if(player2.blockVO != NULL)
		ofNotifyEvent(BLOCKING,*player2.blockVO,this); 
}

void Model::setState(int newValue)
{
	cout << "Model::setState:" << newValue << "\n";
	state = newValue;
	
	gamePaused = true;
	
	switch(state)
	{
		case STATE_COUNT_DOWN:
			countDown = countDownTimer.getRepeatCount();
			countDownTimer.reset();
			countDownTimer.start();
			break;
		case STATE_GAME:
			//if(!gamePaused)
			//soundController.Reset();
			gamePaused = false;
			
			break;
		case STATE_WAITING:
			gamePaused = true;
			break;
		case STATE_GAME_FINISHED:
			soundController.PlaySound(SOUND_WARNING,0);
			finishTimer.start();
			break;
		case STATE_DEMO:
			resetGame();
			break;
	}
	
	//soundController.PlayBGSound(SOUND_BG,player1Health,player2Health,startHealth,state);
	
	cout << "  notifyEvent(STATE_CHANGE\n";
	int emptyArg = 0;
	ofNotifyEvent(STATE_CHANGE,emptyArg,this);
}
void Model::setVideoPause(bool newValue)
{

	videoPaused = newValue;
	
	int emptyArg = 0;
	if(videoPaused)
		ofNotifyEvent(VIDEO_PAUSE,emptyArg,this);
	else
		ofNotifyEvent(VIDEO_RESUME,emptyArg,this);
}
void Model::setDetectedPlayer1(bool newValue)
{	
	if(overulePlayer1Detected && newValue == true) return;
	if(newValue == detectedPlayer1) return;
	cout << "Model::setDetectedPlayer1: " << newValue << "\n";
	
	detectedPlayer1 = newValue;
	
	if(overulePlayer1Detected)
		detectedPlayer1 = false;
	
	int emptyArg = 0;
	ofNotifyEvent(PLAYERS_CHANGED,emptyArg,this);
	
	checkPlayers();
}
void Model::setDetectedPlayer2(bool newValue)
{
	if(overulePlayer2Detected && newValue == true) return;
	if(newValue == detectedPlayer2) return;	
	cout << "Model::setDetectedPlayer2: " << newValue << "\n";

	detectedPlayer2 = newValue;
	
	if(overulePlayer2Detected)
		detectedPlayer2 = false;
	
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
	
//	if(detectedPlayer2 == false || detectedPlayer1 == false)
//		setVideoPause(true);
	
	if(state == STATE_DEMO && detectedPlayer1 && detectedPlayer2)
	{
		setState(STATE_COUNT_DOWN);
	}
	else if(state == STATE_GAME)
	{
		if(!detectedPlayer1 && !detectedPlayer2)
		{
			cout << "    missing both players\n";
			waitingDelayer.start();
			demoDelayer.start();
		}
		else if(!detectedPlayer1 || !detectedPlayer2)
		{
			cout << "    missing one player\n";
			waitingDelayer.start();
			demoDelayer.reset();
		}
		else
		{
			waitingDelayer.reset();
			demoDelayer.reset();
		}
	}
	else if(state == STATE_WAITING)
	{
		if(!detectedPlayer1 && !detectedPlayer2)
		{
			cout << "    missing both players\n";
			demoDelayer.start();
		}
		else if(detectedPlayer1 && detectedPlayer2)
		{
			setState(STATE_GAME);
			demoDelayer.reset();
			waitingDelayer.reset();
		}
		else if(detectedPlayer1 || detectedPlayer2)
		{
			cout << "    missing one player\n";
			demoDelayer.reset();
		}
		
	}
	else if(state != STATE_GAME && state != STATE_GAME_FINISHED  && (!detectedPlayer1 || !detectedPlayer2))
	{
		setState(STATE_DEMO);
		countDownTimer.reset();
	}
}
void Model::checkPlayersPositions()
{
	return;
	if(state != STATE_GAME && state != STATE_WAITING) return;
	cout << "Model::checkPlayersPositions\n";

	int positionTollerance = 10; //5; 
	
	bool detected = false;
	if(player1.mainBody != NULL && player2.mainBody != NULL)
	{
		int player1FrontX = player1.mainBody->x+player1.mainBody->width;
		int player2FrontX = player2.mainBody->x;
		
		cout << "Model::checkPlayersPositions: 1: " << player1FrontX << " 2: " << player2FrontX << " center: " << centerX << "\n";
		
		if(player1FrontX > centerX+positionTollerance) 
		{
			
		}
		else if(player2FrontX < centerX-positionTollerance) 
		{
			
		}
		else
		{
			detected = true;
			
		}
	}
	
	
//	detectedPlayer1 = false;
//	if(player1.mainBody != NULL)
//	{
//		int player1FrontX = player1.mainBody->x+player1.mainBody->width;
//		if(player1FrontX < centerX+positionTollerance) 
//		{
//			detectedPlayer1 = true;
//		}
//	}
//	cout << "  detectedPlayer1: " << detectedPlayer1 << "\n";
//	detectedPlayer2 = false;
//	if(player2.mainBody != NULL)
//	{
//		int player2FrontX = player2.mainBody->x+player2.mainBody->width;
//		if(player2FrontX > centerX-positionTollerance) 
//		{
//			detectedPlayer2 = true;
//		}
//	}
//	cout << "  detectedPlayer2: " << detectedPlayer2 << "\n";
	
	if(detected)
	{
		//soundController.PlaySound(SOUND_WARNING,0);
	}
	else 
	{
		//soundController.PlaySound(SOUND_WARNING,1);
		//setVideoPause(true);
		//setState(STATE_WAITING);
	}
//	cout << "  state: " << state << "\n";
//	if(state == STATE_DEMO)
//	{
//		int emptyArg = 0;
//		ofNotifyEvent(PLAYERS_CHANGED,emptyArg,this);
//	}
//	
//	if(state == STATE_DEMO && detectedPlayer1 && detectedPlayer2)
//	{
//		setState(STATE_COUNT_DOWN);
//	}
//	else if(state == STATE_GAME)
//	{
//		if(!detectedPlayer1 || !detectedPlayer2)
//		{
//			//setState(STATE_WAITING);
//			soundController.PlaySound(SOUND_WARNING,1);
//		}
//		else 
//		{
//			soundController.PlaySound(SOUND_WARNING,0);
//		}
//	}
//	else if(state != STATE_GAME && state != STATE_GAME_FINISHED  && (!detectedPlayer1 || !detectedPlayer2))
//	{
//		setState(STATE_DEMO);
//		countDownTimer.reset();
//	}
}


void Model::update(ofEventArgs & args)
{ 
	if(state != STATE_COUNT_DOWN || (state == STATE_COUNT_DOWN && countDown > 0))
	{
		//cout << "Model::update checked, state: "<<state<<"\n";
		soundController.PlayBGSound(SOUND_BG,player1Health,player2Health,startHealth,state);
	}
}
void Model::onCountDownTick(int  & count)
{
	cout << "Model::onCountDownTick\n";
	cout << "  count: " << count << "\n";
	
	if(count < countDownTimer.getRepeatCount()-1)
	{
		countDown = countDownTimer.getRepeatCount()-2-count;
		cout << "  countdown: "<<countDown<<"\n";
		int emptyArg = 0;
		ofNotifyEvent(COUNT_DOWN,emptyArg,this);
		
		
		if(count >= countDownTimer.getRepeatCount()-2)
		{
			//soundController.PlayStartFightSound();
			//resetGame();
			soundController.PlayBGSound(SOUND_BG,player1Health,player2Health,startHealth,STATE_GAME);
		}
		else
		{
			soundController.PlaySound(SOUND_READY);
		}
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
void Model::onWaitingDelayed(int  & count)
{
	cout << "Model::onWaitingDelayed\n";
	cout << "  count: "<<count<<"\n";
	
	soundController.PlaySound(SOUND_WARNING, 1);
	setState(STATE_WAITING);
}	
void Model::onDemoDelayed(int  & count)
{
	cout << "Model::onDemoDelayed\n";
	cout << "  count: "<<count<<"\n";
	
	setState(STATE_DEMO);
}
