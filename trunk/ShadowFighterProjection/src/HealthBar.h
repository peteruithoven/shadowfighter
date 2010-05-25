/*
 *  HealthBar.h
 *  ShadowFighter
 *
 *  Created by Peter Uithoven on 4/24/10.
 */

#ifndef _HealthBar
#define _HealthBar

#include "ofMain.h"
#include "ofEvents.h"
#include "DisplayObject.h"

class HealthBar : public DisplayObject 
{
	public:
		float percentage;
		ofImage bgImg;
		ofImage barImgOrg;
		
		HealthBar();
		void draw();
	
	private:

};

#endif