/*
 *  DisplayView.cpp
 *  openFrameworks
 *  
 *  Created by Peter Uithoven on 5/30/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 */

#include "VideoInputView.h"

VideoInputView::VideoInputView()
{
	cout << "VideoInputView::VideoInputView\n";
	ofAddListener(videoInput.NEW_PIXELS,this,&VideoInputView::onNewPixels);
}

void VideoInputView::setModel(Model * model)
{
	this->model = model;
	controller.setModel(model);
}
void VideoInputView::onNewPixels(int & nothing)
{
	controller.newPixels(videoInput.videoPlayer.pixels);
}