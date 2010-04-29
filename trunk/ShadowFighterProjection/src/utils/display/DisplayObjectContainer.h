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

class DisplayObjectContainer : public DisplayObject 
{
	public:
		DisplayObjectContainer();
		void addChild(DisplayObject * child);
		void removeChildAt(int index);
		void clear();
		int size();
		DisplayObject * getChildAt(int index);
		void setAutoDraw(bool newValue);

	private:
	
		vector<DisplayObject*>	children;
		void draw(ofEventArgs & args);
};

#endif
