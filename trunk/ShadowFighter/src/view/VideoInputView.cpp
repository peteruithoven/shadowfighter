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
	cout << "VideoInputView::setModel\n";
	this->model = model;
	controller.setModel(model);
	
	ofAddListener(model->DATA_LOADED,this,&VideoInputView::onDataLoaded);
	ofAddListener(model->VIDEO_PAUSE,this,&VideoInputView::onVideoPause);
	ofAddListener(model->VIDEO_RESUME,this,&VideoInputView::onVideoResume);
}
void VideoInputView::onDataLoaded(int & nothing)
{
	videoInput.width = model->videoW;
	videoInput.height = model->videoH;
	
	cout << "  model->pixelsSource: " << model->pixelsSource << "\n";
	cout << "  Model::CAMERA: " << Model::CAMERA << "\n";
	cout << "  model->movieURL: " << model->movieURL << "\n";
	
	bool useCamera = (model->pixelsSource == Model::CAMERA);
	cout << "  useCamera: " << useCamera << "\n";
	
	videoInput.slowMotion = model->slowMotion;
	videoInput.init(useCamera,model->movieURL);
}
void VideoInputView::onVideoPause(int & nothing)
{
	videoInput.pause();
}	
void VideoInputView::onVideoResume(int & nothing)
{
	videoInput.resume();	
}
void VideoInputView::onNewPixels(int & nothing)
{
	if(model->pixelsSource == Model::CAMERA)
		controller.newPixels(videoInput.camera.getPixels());
	else
		controller.newPixels(videoInput.videoPlayer.pixels);
}