/*
 *  DisplayObject.h
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 5/8/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _DisplayObject
#define _DisplayObject

#include <ofUtils.h>
#include <ofMain.h>

class DisplayObject{

	public:
		int x;
		int y;
		int width;
		int height;	
		int rotation;
		
		DisplayObject();
		virtual void draw(ofEventArgs & args);
};

#endif
