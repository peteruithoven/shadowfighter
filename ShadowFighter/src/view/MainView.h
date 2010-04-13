/*
 *  MainView.h
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 4/27/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *	
 *  One of the views. Views are responsible for hooking up 
 *	project indipendent components (gui for example) to the project. 
 *	I will listen to the model for changes in (mainly) data 
 *	and will update itself according to that data.
 */

#ifndef _MainView
#define _MainView

#include "Model.h"
#include "BaseView.h"

class MainView : public BaseView
{
	
public:
	
	MainView();
	
	void setModel(Model * model);
	
protected:
	//void onBeat(int & fake);
};

#endif
