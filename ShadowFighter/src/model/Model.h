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

class Model{

	public:
		ofxXmlSettings		xml;
		int					videoW;
		int					videoH;
		bool				debug;
		int					threshold;
		bool				willLearnBackground;
		int					minBlobSize;
		int					maxBlobSize;
		int					maxNumBlobs;
	
		ofxCvGrayscaleImage* grayImg;
		ofxCvGrayscaleImage* grayEmptyImg;
		ofxCvGrayscaleImage* grayDiffImg;
	
		Model();
		void learnBackground();
		void setThreshold(int newValue);
		void setDebug(bool debug);
		
		ofEvent< int > VALUES_UPDATED;
		
	protected:
		Timer				timer;
	
		void update(ofEventArgs & args);
		void onTick(int  & count);
};

#endif
