/*
 *  Display.h
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 5/30/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _Display
#define _Display

#include <ofUtils.h>
#include <ofTrueTypeFont.h>
#include "ofEvents.h"
#include "DisplayObject.h"
#include <sstream>

class Display : public DisplayObject 
{

	public:
		Display();
		int threshold;
		int hitThreshold;
		void start();
	
	private:
		ofTrueTypeFont ttf;
		void draw(ofEventArgs & args);
};

#endif
