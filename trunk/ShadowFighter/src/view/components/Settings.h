/*
 *  Settings.h
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 11/19/09.
 *  Copyright 2009 HKU. All rights reserved.
 *
 */

#ifndef _Settings
#define _Settings

#include <ofUtils.h>
#include "ofxSimpleGuiToo.h"
#include "Model.h"

class Settings{

	public:
		Settings();
		
		Model * model;
		void setup();
	
	protected:
		ofxSimpleGuiToo	gui;
		void draw(ofEventArgs & args);
};

#endif
