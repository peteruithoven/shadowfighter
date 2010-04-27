/*
 *  HitIndicator.h
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 5/30/09.
 */

#ifndef _HitIndicator
#define _HitIndicator

#include <ofUtils.h>
#include <ofTrueTypeFont.h>
#include "ofEvents.h"
#include "DisplayObject.h"
#include <sstream>

class HitIndicator : public DisplayObject 
{

	public:
		int						imgY;
		ofImage					img;
	
		HitIndicator();
		void start();
		void stop();
		bool running;
		void draw();
	private:
		
};

#endif
