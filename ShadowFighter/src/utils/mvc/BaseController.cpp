/*
 *  BaseController.cpp
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 5/8/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "BaseController.h"

BaseController::BaseController()
{
	
}
Model * BaseController::getModel()
{
	return model;
}
void BaseController::setModel(Model * model)
{
	this->model = model;
}