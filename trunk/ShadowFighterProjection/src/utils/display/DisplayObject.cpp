/*
 *  DisplayObject.cpp
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 5/8/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "DisplayObject.h"

DisplayObject::DisplayObject()
{
	x = 0;
	y = 0;
	alpha = 255;
	width = 0;
	height = 0;
	rotation = 0;
	scale = 1;
	
	
	setAutoDraw(true);

}
void DisplayObject::setAutoDraw(bool newValue)
{
	autoDraw = newValue;
	if(autoDraw)
		ofAddListener(ofEvents.draw, this, &DisplayObject::draw);
	else
		ofRemoveListener(ofEvents.draw, this, &DisplayObject::draw);
}
void DisplayObject::draw(ofEventArgs & args)
{
	draw();
}
void DisplayObject::draw()
{
}