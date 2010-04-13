/*
 *  MainView.cpp
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 4/27/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 */

#include "MainView.h"
#include "ofMain.h"
#include "ofEvents.h"

MainView::MainView()
{
	ofBackground(0,0,0);
}

void MainView::setModel(Model * model)
{
	cout << "MainView::setModel\n";
	this->model = model;
	//ofAddListener(model->BEAT,this,&MainView::onBeat);
}