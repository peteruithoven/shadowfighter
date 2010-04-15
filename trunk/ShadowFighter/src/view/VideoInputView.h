/*
 *  DisplayView.h
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 5/30/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *  
 */

#ifndef _VideoInputView
#define _VideoInputView

#include <ofUtils.h>
#include <Model.h>
#include <Display.h>
#include <BaseView.h>
#include <VideoInput.h>
#include <VideoInputController.h>

class VideoInputView : public BaseView
{

	public:
		VideoInputController controller;
	
		VideoInputView();
		void setModel(Model * model);
	
	private:

		VideoInput videoInput;
		void onNewPixels(int & nothing);
		void onCameraIndexChanged(int & nothing);
};

#endif
