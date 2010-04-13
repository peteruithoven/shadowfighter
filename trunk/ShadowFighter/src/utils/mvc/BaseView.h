/*
 *  BaseView.h
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 5/8/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _BaseView
#define _BaseView

#include <ofUtils.h>
#include <Model.h>

class BaseView
{

	public:
		
		BaseView();
		virtual Model * getModel();
		virtual void setModel(Model * model);	
			
	protected:
		Model * model;
		
};

#endif
