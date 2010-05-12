/*
 *  HitIndicatorHitIndicator.cpp
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 5/30/09.
 */

#include "HitIndicator.h"
#include <sstream>

HitIndicator::HitIndicator()
{
	alphaChange = 10; //20
	
	bangImg.loadImage("images/boom2.png");
	bangImgY = 0-bangImg.getHeight();
	
	hitImg.loadImage("images/hit.png");
	hitOrgW = hitImg.getWidth();
	hitOrgH = hitImg.getHeight();
	hitScale = 0.3;
}
void HitIndicator::start()
{
	width = 0;
	running = true;
}
void HitIndicator::stop()
{
	running = false;
}

void HitIndicator::draw()
{ 
	if(running)
	{
		ofFill();
		ofEnableAlphaBlending();
		ofSetColor(255,255,255,alpha);
		//ofSetColor(255,255,255,255);
		//ofCircle(x, y, width*scale);
		//bangImg.draw(x-bangImg.getWidth()/2, y+bangImgY);
		
		ofPushMatrix();
		ofTranslate(x, y, 0);
		ofScale(hitScale, hitScale, 1);
		
		hitImg.draw(0-hitImg.getWidth()/2,0-hitImg.getHeight()/2);
		
		ofPopMatrix();
		
		ofSetColor(255,255,255,255);
		ofDisableAlphaBlending();
		
		width += 8;
		alpha -= alphaChange;
		bangImgY -= 2;
		hitScale += 0.02;
		if(alpha < 0)
			running = false;
	}
}