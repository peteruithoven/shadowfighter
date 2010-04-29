/*
 *  Image.h
 *  ShadowFighterProjection
 *
 *  Created by Peter Uithoven on 4/28/10.
 */

#ifndef _Image
#define _Image

#include "ofMain.h"
#include "ofEvents.h"
#include "DisplayObject.h"

class Image : public DisplayObject 
{
	public:
		Image();
		ofImage img;
	
	protected:
		void draw();
};

#endif