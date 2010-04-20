/*
 *  SettingsView.cpp
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 4/12/10.
 *  Copyright 2010 HKU. All rights reserved.
 *
 */

#include "ProjectionView.h"

ProjectionView::ProjectionView()
{
	
}

void ProjectionView::setModel(Model * model)
{
	cout << "ProjectionView::setModel\n";
	this->model = model;
	ofAddListener(model->HIT,this,&ProjectionView::onHit);
	projection.model = model;
}
void ProjectionView::onHit(int & arg)
{
	//cout << "ProjectionView::onHit\n";
	int x = model->hitRect->x+model->hitRect->width/2;
	int y = model->hitRect->y+model->hitRect->height/2;
	projection.addHit(x, y);
}