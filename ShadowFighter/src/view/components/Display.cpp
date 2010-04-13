/*
 *  Display.cpp
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 5/30/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "Display.h"
#include <sstream>

Display::Display()
{
	threshold = 0;
	ttf.loadFont("mono.ttf", 7);
	ofAddListener(ofEvents.draw, this, &Display::draw);
}
void Display::draw(ofEventArgs & args)
{ 
	/*ofSetColor(200, 200, 200);
	
	int x = 20;
	int y = 20;
	
	std:string displayStr = "";
	displayStr += "Threshold: ";
	displayStr += ofToString(threshold);
	displayStr += "\n";
	displayStr += "flowValue: ";
	displayStr += ofToString(flowValue);
	displayStr += "\n";
	displayStr += "flowIntervalRatio: ";
	displayStr += ofToString(flowIntervalRatio);
	displayStr += "\n";
	displayStr += "baseInterval: ";
	displayStr += ofToString(baseInterval);
	displayStr += "\n";
	displayStr += "Interval: ";
	displayStr += ofToString(interval);
	displayStr += "\n";
	
	//cout << "displayStr: " << displayStr << endl;
	
	ttf.drawString(displayStr, x, y);*/
}