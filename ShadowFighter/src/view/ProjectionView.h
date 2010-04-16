/*
 *  SettingsView.h
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 4/12/10.
 *  Copyright 2010 HKU. All rights reserved.
 *
 */

#ifndef _ProjectionView
#define _ProjectionView

#include "ofMain.h"
#include "projection.h"
#include "BaseView.h"

class ProjectionView : public BaseView
{

	public:
		ProjectionView();
	
		void setModel(Model * model);
	
	protected:
		Projection projection;
		void onHit(int & arg);
};

#endif
