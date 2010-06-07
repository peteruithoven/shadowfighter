/*
 *  HitVO.h
 *  ShadowFighter
 *
 *  Created by Peter Uithoven on 5/26/10.
 */

#ifndef _HitVO
#define _HitVO

#include "ofMain.h"

class HitVO
{
	public:
		ofRectangle bounds;
		int victim;
		int type;
		int area;
		int attackType;
};

#endif
