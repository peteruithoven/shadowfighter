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
	//ofAddListener(model->HIT,this,&ProjectionView::onHit);
	ofAddListener(model->RESET,this,&ProjectionView::onReset);
	projection.model = model;
}
void ProjectionView::onHit(int & arg)
{
	//cout << "ProjectionView::onHit\n";
	int x = model->hitRect->x+model->hitRect->width/2;
	int y = model->hitRect->y+model->hitRect->height/2;
	projection.addHit(x, y);
	
	updateHealth();
}
void ProjectionView::onReset(int & arg)
{
	updateHealth();
}

void ProjectionView::updateHealth()
{
	float player1Health = float(model->player1Health)/model->startHealth;
	float player2Health = float(model->player2Health)/model->startHealth;
	projection.updateHealth(player1Health, player2Health);
}