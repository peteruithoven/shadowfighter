/*
 *  BlockIndicator.cpp
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 5/30/09.
 */

#include "BlockIndicator.h"

BlockIndicator::BlockIndicator()
{
	img.loadImage("images/block/block.png");
	
}
void BlockIndicator::start()
{
	cout << "BlockIndicator::start\n";
	imgX = 30+45;
	scale = 0.8;
	alpha = 255;
	running = true;
}
void BlockIndicator::draw()
{ 
	//cout << "BlockIndicator::draw\n";
	
	ofFill();
	ofEnableAlphaBlending();
	ofSetColor(255,255,255,alpha);
	
	//float initScale = height/img.getHeight();
	
	ofPushMatrix();
	ofTranslate(x, y, 0);
	float scaleX = (mirrorX)? -scale : scale;
	ofScale(scaleX, scale, 1);
	img.draw(0-img.getWidth()/2+imgX,0-img.getHeight()/2);
	
	ofPopMatrix();
	
	ofSetColor(255,255,255,255);
	ofDisableAlphaBlending();
	
	//ofCircle(x, y, 10);
	
	alpha -= 20;
	scale += 0.01;
	if(alpha < 0)
		stop();
}