/*
 *  DisplayView.h
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 5/30/09.
 *  
 *  Displays textual config data. 
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
		void update();
		void onDataLoaded(int & nothing);
		void onStateChange(int & nothing);
		void onValuesUpdated(int & arg);
};

#endif
