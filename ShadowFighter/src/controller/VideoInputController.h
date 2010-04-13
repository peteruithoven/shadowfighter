/*
 *  VideoInputController.h
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 4/13/10.
 *  Copyright 2010 HKU. All rights reserved.
 *
 */

#ifndef _VideoInputController
#define _VideoInputController

#include <ofUtils.h>
#include "BaseController.h"

class VideoInputController : public BaseController
{
	public:
		VideoInputController();
		void newPixels(unsigned char * pixels);
	
	protected:
};

#endif
