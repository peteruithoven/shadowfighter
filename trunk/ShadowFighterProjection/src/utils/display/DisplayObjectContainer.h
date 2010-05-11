/*
 *  DisplayObjectContainer.h
 *  ShadowFighterProjection
 *
 *  Created by Peter Uithoven on 4/28/10.
 */

#ifndef _DisplayObjectContainer
#define _DisplayObjectContainer

#include "ofMain.h"
#include "ofEvents.h"
#include "DisplayObject.h"
#include "Image.h"


class DisplayObjectContainer : public DisplayObject 
{
	public:
		DisplayObjectContainer();
		DisplayObject* addChild(DisplayObject * child);
		void removeChildAt(int index);
		void clear();
		int size();
		DisplayObject * getChildAt(int index);
		void draw();
	
	private:
		vector<DisplayObject*>	children;
};

#endif
