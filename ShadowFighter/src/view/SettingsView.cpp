/*
 *  SettingsView.cpp
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 4/12/10.
 *  Copyright 2010 HKU. All rights reserved.
 *
 */

#include "SettingsView.h"

SettingsView::SettingsView()
{
	
}

void SettingsView::setModel(Model * model)
{
	cout << "SettingsView::setModel\n";
	this->model = model;
	
	settings.model = this->model;
	settings.setup();
}