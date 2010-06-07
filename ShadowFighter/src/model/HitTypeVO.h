//
//  HitTypeVO.h
//  ShadowFighter
//
//  Created by Peter Uithoven on 6/7/10.
//  Copyright 2010 Peter Uithoven. All rights reserved.
//

#ifndef _HitTypeVO
#define _HitTypeVO

#include "ofMain.h"

class HitTypeVO
{
public:
	int name;
	float maxY;
	int damage;
	
	HitTypeVO(int _name, float _maxY, int _damage)
	{
		name = _name;
		maxY = _maxY;
		damage = _damage;
	}
	
};

#endif