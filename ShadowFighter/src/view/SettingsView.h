/*
 *  SettingsView.h
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 4/12/10.
 *  Copyright 2010 HKU. All rights reserved.
 *
 */

#ifndef _SettingsView
#define _SettingsView

#include <ofUtils.h>
#include "Settings.h"
#include "BaseView.h"

class SettingsView : public BaseView
{

	public:
		SettingsView();
	
		void setModel(Model * model);
	
	protected:
		Settings settings;
};

#endif
