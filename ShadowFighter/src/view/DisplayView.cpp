/*
 *  DisplayView.cpp
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 5/30/09.
 *
 */

#include "DisplayView.h"

DisplayView::DisplayView()
{
	
}

void DisplayView::setModel(Model * model)
{
	this->model = model;
	
	ofAddListener(model->DATA_LOADED,this,&DisplayView::onDataLoaded);
	ofAddListener(model->STATE_CHANGE,this,&DisplayView::onStateChange);
	
	
}
void DisplayView::onDataLoaded(int & nothing)
{
	ofAddListener(model->VALUES_UPDATED,this,&DisplayView::onValuesUpdated);
	display.start();
	update();
}
void DisplayView::onStateChange(int & nothing)
{
	update();
}

void DisplayView::onValuesUpdated(int & arg)
{
	update();
}
void DisplayView::update()
{
	cout << "DisplayView::update\n";
	display.threshold = model->threshold;
	display.hitThreshold = model->hitThreshold;
	display.state = model->state;
}