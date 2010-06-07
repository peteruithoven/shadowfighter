/*
 *  BlockingIndicator.cpp
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 5/30/09.
 */

#include "BlockingIndicator.h"

BlockingIndicator::BlockingIndicator()
{
	img.loadImage("images/blocking/blocking.png");
	
}
void BlockingIndicator::start()
{
	//cout << "BlockingIndicator::start\n";
	scale = 1;
	alpha = 255;
	running = true;
}
void BlockingIndicator::draw()
{ 
	//cout << "BlockingIndicator::draw\n";
	
	ofFill();
	ofEnableAlphaBlending();
	ofSetColor(255,255,255,alpha);
	
	//float initScale = height/img.getHeight();
	
	ofPushMatrix();
	ofTranslate(x, y, 0);
	float scaleX = (mirrorX)? -scale : scale;
	ofScale(scaleX, scale, 1);
	
	img.draw(0-img.getWidth()/2+30,0-img.getHeight()/2);
	
	ofPopMatrix();
	
	ofSetColor(255,255,255,255);
	ofDisableAlphaBlending();
	
	//ofCircle(x, y, 10);
	
	alpha -= 25; //17;
	//scale += 0.01;
	if(alpha < 0)
		stop();
	//stop();
}