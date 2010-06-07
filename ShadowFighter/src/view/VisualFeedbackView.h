/*
 *  SettingsView.h
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 4/12/10.
 *  Copyright 2010 HKU. All rights reserved.
 *
 */

#ifndef _VisualFeedbackView
#define _VisualFeedbackView

#include "ofMain.h"
#include "Settings.h"
#include "BaseView.h"
#include "VisualFeedback.h"

class VisualFeedbackView : public BaseView
{

	public:
		VisualFeedbackView();
	
		void setModel(Model * model);
	
	protected:
		VisualFeedback visualFeedback;
		void onHit(HitVO & hitVO);
};

#endif
