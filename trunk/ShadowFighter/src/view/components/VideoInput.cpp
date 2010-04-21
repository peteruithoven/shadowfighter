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
	useCamera = false;
	
	width = 640;
	height = 480;
	
	ofAddListener(ofEvents.draw, this, &VideoInput::draw);
	if(useCamera)
	{
		camera.setDeviceID(4);
		camera.setVerbose(true);
		camera.initGrabber(width,height);
		
		colorImg.allocate(width,height);
	}
	else 
	{
		videoPlayer.loadMovie("movies/clip1.mov");
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