/*
 *  PlayerVO.h
 *  ShadowFighter
 *
 *  Created by Peter Uithoven on 5/20/10.
 */

#ifndef _PlayerVO
#define _PlayerVO

#include "ofMain.h"
#include "BlockVO.h"

class PlayerVO
{
	public:
		
		ofRectangle *body;
		ofRectangle *mainBody;
		ofRectangle *block;
		BlockVO		*blockVO;
		ofRectangle *area;
		bool powerHit;
};

#endif
