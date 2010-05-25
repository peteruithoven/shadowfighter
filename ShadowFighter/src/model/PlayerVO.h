/*
 *  PlayerVO.h
 *  ShadowFighter
 *
 *  Created by Peter Uithoven on 5/20/10.
 */

#ifndef _PlayerVO
#define _PlayerVO

#include "ofMain.h"

class PlayerVO
{
	public:
		
		ofRectangle *body;
		ofRectangle *block;
		ofRectangle *area;
	
		PlayerVO();
};

#endif
