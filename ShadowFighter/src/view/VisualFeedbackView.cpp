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
void VisualFeedbackView::onHit(HitVO & hitVO)
{
	ofRectangle bounds = hitVO.bounds;
	int x = bounds.x+bounds.width/2;
	int y = bounds.y+bounds.height/2;
	visualFeedback.addHit(x, y);
}