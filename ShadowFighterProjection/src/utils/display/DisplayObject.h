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

class DisplayObject
{
	public:
		float x;
		float y;
		float width;
		float height;
		float alpha;
		float rotation;
		float scale;
	
		bool autoDraw;
	
		DisplayObject();
		~DisplayObject();
		void setAutoDraw(bool newValue);
		virtual void draw();
		virtual void destroy();
	
	protected:		
		void draw(ofEventArgs & args);
	
};

#endif