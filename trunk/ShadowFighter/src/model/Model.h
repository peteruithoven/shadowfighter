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

class Model
{
	public:
		
		static int			CAMERA;
		static int			CLIP1_DEMO;
		static int			CLIP5_DEMO;
	
		ofxXmlSettings		xml;
		int					videoW;
		int					videoH;
		int					pixelsSource;
		string				movieURL;
		ofRectangle*		hitRect;
		int					cameraIndex;
		int					state;
		
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
		string				backgroundImageURL;
		ofRectangle			detectionZone;
		ofRectangle			hitDetectionZone;
		
		ofxCvGrayscaleImage* grayImg;
		ofxCvGrayscaleImage* grayEmptyImg;
		ofxCvGrayscaleImage* grayDiffImg;
		ofxCvGrayscaleImage* grayHitDiffImg;
		
		ofImage*			imgLoader;
		
		vector<ofxCvBlob*>*	prevHitBlobs;
		vector< vector<Blob*>* >*	blobsHistory;
		ofxCvGrayscaleImage* prevGrayDiffImg;
		int					maxBlobsHistoryLength;
	
		// game logic
		int					startHealth;
		int					player1Health;
		int					player2Health;
		int					hitDamage;
	
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
		bool				slowMotion;
		Model();
		void loadData();
		void start();
		void learnBackground();
		void setThreshold(int newValue);
		void setHitThreshold(int newValue);
		void setDebug(bool debug);
		void setState(int state);
	
		void hit(int type, int area, int victim);
		
		ofEvent< int > DATA_LOADED;
		ofEvent< int > VALUES_UPDATED;
		ofEvent< int > HIT;
		ofEvent< int > STATE_CHANGE;
		ofEvent< int > RESET;
	protected:
		Timer				timer;
	
		void parseXML();
		void storeValues();
		void resetGame();
		void update(ofEventArgs & args);
		void onTick(int  & count);
};

#endif
