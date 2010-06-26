/*
 *  Display.cpp
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 5/30/09.
 */

#include "Display.h"
#include <sstream>

Display::Display()
{
	threshold = 0;
	hitThreshold = 0;
	
	ttf.loadFont("mono.ttf", 10);
}
void Display::start()
{
	ofAddListener(ofEvents.draw, this, &Display::draw);
}
void Display::draw(ofEventArgs & args)
{ 
	//ofSetColor(200, 200, 200);
	ofSetColor(220, 220, 220);
	
	int x = 20;
	int y = 20;
	
	std:string displayStr = "";
	displayStr += "Threshold: " + ofToString(threshold) + "\n";
	displayStr += "Hit threshold: " + ofToString(hitThreshold) + "\n";	
	displayStr += "Framerate: " + ofToString(ofGetFrameRate(), 2) + "fps\n";
	
	string stateName = "";
	switch (state) {
		case 0:
			stateName = "DEMO";
			break;
		case 1:
			stateName = "WAITING";
			break;
		case 2:
			stateName = "COUNT_DOWN";
			break;
		case 3:
			stateName = "GAME";
			break;
		case 4:
			stateName = "GAME_FINISHED";
			break;
		default:
			break;
	}
	
	displayStr += "State: " + stateName + "\n";	
	//cout << "displayStr: " << displayStr << endl;
	
	ttf.drawString(displayStr, x, y);
}