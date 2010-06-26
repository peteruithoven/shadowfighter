/*
 *  HitIndicator.h
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 5/30/09.
 */

#ifndef _HitIndicator
#define _HitIndicator

#include "ofMain.h";
#include "Effect.h"
#include "Constants.h"

class HitIndicator : public Effect 
{

	public:
		int						bangImgY;
		int						alphaVelocity;
		float					scaleVelocity;
		float					textScaleVelocity;
		float					hitOrgW;
		float					hitOrgH;
		float					hitScale;
		float					textScale;
		float					textX;
		float					textY;
		float					minRotation;
		float					maxRotation;
		ofImage					bangImg;
		ofImage					hitImg;
		ofImage					textImg;
	
		HitIndicator(int hitType);
		void start();
		void draw();
	
	private:
		
};

#endif
