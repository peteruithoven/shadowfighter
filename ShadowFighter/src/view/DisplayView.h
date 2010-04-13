/*
 *  DisplayView.h
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 5/30/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *  
 *  Displays textual data. 
 */

#ifndef _DisplayView
#define _DisplayView

#include <ofUtils.h>
#include "Model.h"
#include "Display.h"
#include "BaseView.h"

class DisplayView : public BaseView
{

	public:
		DisplayView();
		void setModel(Model * model);
	private:

		Display display;
		void onValuesUpdated(int & arg);
};

#endif
