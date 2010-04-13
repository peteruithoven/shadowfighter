/*
 *  BaseView.cpp
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 5/8/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "BaseView.h"
#include "Model.h"

BaseView::BaseView()
{
	
}
Model * BaseView::getModel()
{
	return this->model;
}
void BaseView::setModel(Model * model)
{
	this->model = model;
}