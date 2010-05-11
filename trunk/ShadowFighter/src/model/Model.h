/*
 *  Model.h
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 4/27/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _Model
#define _Model

#include "ofEvents.h"
#include "ofxXmlSettings.h"
#include "Timer.h"
#include "ofxCvGrayscaleImage.h"
#include "ofImage.h"
#include "Blob.h"
#include "Constants.h"
#include "SoundController.h"

class Model
{
	public:
		
		static int			CAMERA;
		static int			CLIP1_DEMO;
		static int			CLIP5_DEMO;
		static int			CLIP6_DEMO;
	
		ofxXmlSettings		xml;
		int					videoW;
		int					videoH;
		int					pixelsSource;
		string				movieURL;
		ofRectangle*		hitRect;
		int					cameraIndex;
		int					state;
		int					countDown;
	
		SoundController		soundController;
	
		// detection
		int					blobDiffTolerance;
		int					threshold;
		int					hitThreshold;
		bool				willLearnBackground;
		int					minBlobArea;
		int					maxHitBlobArea;
		int					minHitBlobArea;
		int					maxBlobArea;
		int					maxNumBlobs;
		int					maxNumHitBlobs;
		int					minDiffHitBlobsPos;
		int					minAttackSpeed;
		int					clip6EmptyCorrection;
		bool				simpleHitBlobAnalysis;
		int					centerX;
		bool				detectedPlayer1;
		bool				detectedPlayer2;
		
		string				backgroundImageURL;
		ofRectangle			detectionZone;
		ofRectangle			hitDetectionZone;
		
		ofxCvGrayscaleImage* grayImg;
		ofxCvGrayscaleImage* grayEmptyImg;
		ofxCvGrayscaleImage* grayEmptyCopyImg;
		ofxCvGrayscaleImage* grayDiffImg;
		ofxCvGrayscaleImage* prevGrayDiffImg;
		ofxCvGrayscaleImage* grayHitDiffImg;
		ofImage*			imgLoader;
		
		vector<ofxCvBlob*>*	prevHitBlobs;
		vector<Blob*> *				currentBlobs; //TODO remove
		vector< vector<Blob*>* >*	blobsHistory;
		
		int					maxBlobsHistoryLength;
		
		// game logic
		bool				gamePaused;
		int					startHealth;
		int					player1Health;
		int					player2Health;
		int					hitDamage;
		int					winner;
	
		// debugging
		bool				debug;
		bool				debugDetection;
		int					possibleAttacksCounter;
		int					hitCounter;
		int					frameCounter;
		int					hitsTextX;
		int					hitsTextY;
		int					attacksTextX;
		int					attacksTextY;
		int					lineHeight;
		bool				takeScreenShots;
		bool				takeHitScreenShots;
		bool				slowMotion;
		bool				videoPaused;
		
		Model();
		void loadData();
		void start();
		void learnBackground();
		void setThreshold(int newValue);
		void setHitThreshold(int newValue);
		void setDebug(bool debug);
		void setState(int state);
		void setVideoPause(bool newValue);
		void setDetectedPlayer1(bool newValue);
		void setDetectedPlayer2(bool newValue);
		void checkPlayers();
		void hit(int type, int area, int victim);
		
		ofEvent< int > DATA_LOADED;
		ofEvent< int > VALUES_UPDATED;
		ofEvent< int > HIT;
		ofEvent< int > STATE_CHANGE;
		ofEvent< int > RESET;
		ofEvent< int > VIDEO_PAUSE;
		ofEvent< int > VIDEO_RESUME;
		ofEvent< int > PLAYERS_CHANGED;
		ofEvent< int > COUNT_DOWN;
		
	protected:
		Timer				countDownTimer;
		Timer				finishTimer;
	
		void parseXML();
		void storeValues();
		void resetGame();
		void update(ofEventArgs & args);
		void onCountDownTick(int  & count);
		void onFinishTick(int  & count);	
};

#endif
