/*
 *  HitIndicatorHitIndicator.cpp
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 5/30/09.
 */

#include "HitIndicator.h"

HitIndicator::HitIndicator(int hitType)
{
	alphaChange = 10; //20
	
	if(hitType == HIT_POWER)
	{
		hitImg.loadImage("images/hit/POWERHIT_1.png");	   
	}
	else 
	{
		hitImg.loadImage("images/hit/hit.png");	   		
	}
	hitOrgW = hitImg.getWidth();
	hitOrgH = hitImg.getHeight();
	
	textX = 0;
	textY = 0;
	
	switch(hitType)
	{
		case HIT_HEAD:
			textImg.loadImage("images/hit/HIT_head_1.png");	
			textX = ofRandom(-50,-100);
			textY = ofRandom(-50,-100);
			break;
		case HIT_BODY:
			textImg.loadImage("images/hit/HIT_body_1.png");	   		
			textX = ofRandom(-50,-100);
			textY = ofRandom(-50,-100);
			break;
		case HIT_LEGS:
			textImg.loadImage("images/hit/HIT_legs_1.png");	   	
			textX = ofRandom(-50,-100);
			textY = ofRandom(-50,-100);
			break;
		case HIT_POWER:	
			break;
	}
	
	textX -= textImg.getWidth()/2;
	textY -= textImg.getHeight()/2;
		
	start();
}
void HitIndicator::start()
{
	width = 0;
	bangImgY = 0-bangImg.getHeight();
	hitScale = 0.4;
	rotation = ofRandom(-45,45);
	alpha = 200;
	running = true;
}
void HitIndicator::draw()
{ 
	//cout << "HitIndicator::draw\n";
	
	ofFill();
	ofEnableAlphaBlending();
	ofSetColor(255,255,255,alpha);
	//ofSetColor(255,255,255,255);
	//ofCircle(x, y, width*scale);
	//bangImg.draw(x-bangImg.getWidth()/2, y+bangImgY);
	
	ofPushMatrix();
	ofTranslate(x, y, 0);
	ofScale(hitScale, hitScale, 1);
	ofRotate(rotation);
	hitImg.draw(0-hitImg.getWidth()/2,0-hitImg.getHeight()/2);
	textImg.draw(textX,textY);
	ofPopMatrix();
	
	ofSetColor(255,255,255,255);
	ofDisableAlphaBlending();
	
	//ofCircle(x, y, 10);
	
	width += 8;
	alpha -= alphaChange;
	bangImgY -= 2;
	hitScale += 0.05;
	if(alpha < 0)
		stop();
}