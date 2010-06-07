/*
 *  BlockVO.h
 *  ShadowFighter
 *
 *  Created by Peter Uithoven on 5/26/10.
 */

#ifndef _BlockVO
#define _BlockVO

#include "ofMain.h"

class BlockVO
{
	public:
		bool mirrorX;
		ofRectangle bounds;
		int victim;
		int type;
		int area;
		int attackType;
};

#endif
