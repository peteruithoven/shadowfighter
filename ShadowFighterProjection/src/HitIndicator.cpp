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
	
}
void HitIndicator::start()
{
	alpha = 200;
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
		ofCircle(x, y, width);
		ofDisableAlphaBlending();
		
		
		width += 5;
		alpha -= 20;
		if(alpha < 0)
			running = false;
	}
}