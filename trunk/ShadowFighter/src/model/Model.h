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

class Model{
	public:
		
		static int					CAMERA;
		static int					CLIP1_DEMO;
	
		ofxXmlSettings		xml;
		int					videoW;
		int					videoH;
		bool				debug;
		int					threshold;
		int					hitThreshold;
		bool				willLearnBackground;
		int					minBlobArea;
		int					maxHitBlobArea;
		int					minHitBlobArea;
		int					maxBlobArea;
		int					maxNumBlobs;
		int					maxNumHitBlobs;
		bool				debugDetection;
		int					minDiffHitBlobsPos;
		string				backgroundImageURL;
		ofRectangle			detectionZone;
		ofRectangle			hitDetectionZone;
		
		int					pixelsSource;
		string				movieURL;
		ofxCvGrayscaleImage* grayImg;
		ofxCvGrayscaleImage* grayEmptyImg;
		ofxCvGrayscaleImage* grayDiffImg;
		
		ofImage*			imgLoader;
		
		vector<ofxCvBlob*>*	blobs;
		vector<ofxCvBlob*>*	prevHitBlobs;
		ofxCvGrayscaleImage* prevGrayDiffImg;
		bool				hitting;
		int					hitCounter;
		ofRectangle*		hitRect;
		int					cameraIndex;
	
		Model();
		void loadData();
		void start();
		void learnBackground();
		void setThreshold(int newValue);
		void setHitThreshold(int newValue);
		void setCameraIndex(int newValue);
		void setDebug(bool debug);
		void hit();
		

		ofEvent< int > DATA_LOADED;
		ofEvent< int > VALUES_UPDATED;
		ofEvent< int > CAMERA_INDEX_CHANGED;
		ofEvent< int > HIT;
		
	protected:
		Timer				timer;
	
		void parseXML();
		void storeValues();
		void update(ofEventArgs & args);
		void onTick(int  & count);
};

#endif
