/*
 *  HealthBar.cpp
 *  ShadowFighter
 *
 *  Created by Peter Uithoven on 4/24/10.
 */

#import "HealthBar.h"


HealthBar::HealthBar()
{
	percentage = 1;
	width = 150;
	height = 10;
	
	ofAddListener(ofEvents.draw, this, &HealthBar::draw);
}

void HealthBar::draw(ofEventArgs & args)
{ 
	ofNoFill();
	ofSetColor(0xffffff);
	ofSetLineWidth(2);
	ofRect(x, y, width, height);
	ofSetLineWidth(1);
	
	ofFill();
	ofRect(x, y, width*percentage, height);
}
