/*
 *  SettingsView.cpp
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 4/12/10.
 *  Copyright 2010 HKU. All rights reserved.
 *
 */

#include "VisualFeedbackView.h"

VisualFeedbackView::VisualFeedbackView()
{
	
}

void VisualFeedbackView::setModel(Model * model)
{
	cout << "VisualFeedbackView::setModel\n";
	this->model = model;
	ofAddListener(model->HIT,this,&VisualFeedbackView::onHit);
}
void VisualFeedbackView::onHit(int & arg)
{
	int x = model->hitRect->x+model->hitRect->width/2;
	int y = model->hitRect->y+model->hitRect->height/2;
	visualFeedback.addHit(x, y);
}