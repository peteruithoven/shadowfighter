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

class Model{

	public:
		ofxXmlSettings		xml;
		bool				debug;
		int					threshold;
		bool				autoThreshold;
		Model();
		
		bool getDebug();
		void setDebug(bool debug);
		
		ofEvent< int > VALUES_UPDATED;
		
	protected:
		Timer timer;
		void update(ofEventArgs & args);
		void onTick(int  & count);
};

#endif
