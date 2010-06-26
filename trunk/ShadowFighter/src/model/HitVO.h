/*
 *  HitVO.h
 *  ShadowFighter
 *
 *  Created by Peter Uithoven on 5/26/10.
 */

#ifndef _HitVO
#define _HitVO

#include "ofMain.h"
#include "HitTypeVO.h"

class HitVO
{
	public:
		ofRectangle bounds;
		int victim;
		HitTypeVO *typeVO;
		int area;
		int attackType;
};

#endif
