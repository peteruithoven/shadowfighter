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
	img.loadImage("images/boom2.png");
	imgY = 0-img.getHeight();
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
		ofCircle(x, y, width*scale);
		img.draw(x-img.getWidth()/2, y+imgY);
		ofDisableAlphaBlending();
		
		width += 8;
		alpha -= 20;
		imgY -= 2;
		if(alpha < 0)
			running = false;
	}
}