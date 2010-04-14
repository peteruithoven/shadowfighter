/*
 *  KeyboardView.h
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 5/8/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _KeyboardView
#define _KeyboardView

#include <ofUtils.h>
#include "KeyboardController.h"
#include "BaseView.h"

class KeyboardView : public BaseView
{

	public:
	
		KeyboardController controller;
		
		KeyboardView();
		
		void setModel(Model * model);
		
		void keyPressed(ofKeyEventArgs & args);
		void keyReleased(ofKeyEventArgs & args);

};

#endif
