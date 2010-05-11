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
DisplayObject::~DisplayObject()
{
	setAutoDraw(false);
}
void DisplayObject::setAutoDraw(bool newValue)
{
	//cout << "DisplayObject::setAutoDraw: " << newValue << "\n";
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
void DisplayObject::destroy()
{
	// Override to clear memory. Call the clear function on ofImage for example.
}