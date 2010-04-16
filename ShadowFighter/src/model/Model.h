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
		ofxXmlSettings		xml;
		int					videoW;
		int					videoH;
		bool				debug;
		int					threshold;
		int					hitThreshold;
		bool				willLearnBackground;
		int					minBlobSize;
		int					maxBlobSize;
		int					maxNumBlobs;
		bool				debugDetection;
		int					minDiffHitBlobsPos;
	
	
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
		void learnBackground();
		void setThreshold(int newValue);
		void setHitThreshold(int newValue);
		void setCameraIndex(int newValue);
		void setDebug(bool debug);
		void hit();
		ofEvent< int > VALUES_UPDATED;
		ofEvent< int > CAMERA_INDEX_CHANGED;
		ofEvent< int > HIT;
		
	protected:
		Timer				timer;
	
		void update(ofEventArgs & args);
		void onTick(int  & count);
};

#endif
