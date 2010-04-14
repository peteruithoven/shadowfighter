/*
 *  KeyboardController.h
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 5/8/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _KeyboardController
#define _KeyboardController

#include <ofUtils.h>
#include "BaseController.h"

class KeyboardController : public BaseController
{

	public:
		
		KeyboardController();
		void keyPressed(int keyCode);
		void keyReleased(int keyCode);
};

#endif
