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
	ofAddListener(model->CAMERA_INDEX_CHANGED,this,&VideoInputView::onCameraIndexChanged);
}
void VideoInputView::onNewPixels(int & nothing)
{
	if(videoInput.useCamera)
		controller.newPixels(videoInput.camera.getPixels());
	else
		controller.newPixels(videoInput.videoPlayer.pixels);
}
void VideoInputView::onCameraIndexChanged(int & nothing)
{
	videoInput.camera.setDeviceID(model->cameraIndex);
}