/*
 *  VideoInput.cpp
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 4/12/10.
 *  Copyright 2010 HKU. All rights reserved.
 *
 */

#include "VideoInput.h"

VideoInput::VideoInput()
{
	//cout << "VideoInput::VideoInput\n";
}
void VideoInput::init(bool bUseCamera,string movieURL)
{
	cout << "VideoInput::init\n";
	cout << "  bUseCamera: " << bUseCamera << "\n";
	cout << "  movieURL: " << movieURL << "\n";
	ofAddListener(ofEvents.draw, this, &VideoInput::draw);
	useCamera = bUseCamera;
	if(useCamera)
	{
		cout << "  initializing camera\n";
		camera.setDeviceID(4);
		camera.setVerbose(true);
		camera.initGrabber(width,height);
		
		colorImg.allocate(width,height);
	}
	else 
	{
		cout << "  initializing video\n";
		videoPlayer.loadMovie(movieURL);
		start();
	}
}
void VideoInput::start()
{
	cout << "VideoInput::start\n";
	videoPlayer.play();
	videoPlayer.setPosition(0.4);
}
void VideoInput::stop()
{
	videoPlayer.stop();
}
void VideoInput::draw(ofEventArgs & args)
{ 
	//cout << "VideoInput::draw\n";
	ofSetColor(255, 255, 255);
	if(useCamera)
	{
		camera.grabFrame();
		colorImg.setFromPixels(camera.getPixels(), width,height);
		colorImg.draw(0,0);
	}
	else
	{
		float position = videoPlayer.getPosition();
		if(position <= 0.1)
			videoPlayer.setPosition(0.4);
		
		videoPlayer.draw(0,0);
	}
	int nothing = 0;
	ofNotifyEvent(NEW_PIXELS,nothing,this);
}