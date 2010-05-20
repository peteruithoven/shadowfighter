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
	running = false;
	paused = false;
	ofAddListener(ofEvents.draw, this, &VideoInput::draw);
}
void VideoInput::init(bool bUseCamera,string movieURL)
{
	cout << "VideoInput::init\n";
	cout << "  bUseCamera: " << bUseCamera << "\n";
	cout << "  movieURL: " << movieURL << "\n";
	
	//slowMotion = false;
	useCamera = bUseCamera;
	if(useCamera)
	{
		cout << "  initializing camera\n";
		camera.setDeviceID(4);
		camera.setVerbose(true);
		camera.initGrabber(width,height);
		//camera.videoSettings();
		colorImg.allocate(width,height);
	}
	else 
	{
		cout << "  initializing video\n";
		videoPlayer.loadMovie(movieURL);
		start();
	}
	running = true;
}
void VideoInput::start()
{
	cout << "VideoInput::start\n";
	videoPlayer.setLoopState(OF_LOOP_NONE);
	if(slowMotion)
		videoPlayer.setSpeed(0.2);
	videoPlayer.setVolume(0);
	videoPlayer.play();
	//videoPlayer.setPosition(0.41);
}
void VideoInput::stop()
{
	if(useCamera) return;
	videoPlayer.stop();
}
void VideoInput::pause()
{
	
	if(useCamera) 
		paused = true;
	else
		videoPlayer.setPaused(true);
}
void VideoInput::resume()
{
	if(useCamera) 
		paused = false;
	else
		videoPlayer.setPaused(false);
}

void VideoInput::draw(ofEventArgs & args)
{ 
	//cout << "VideoInput::draw\n";
	if(!running) return;
	ofSetColor(255, 255, 255);
	if(useCamera)
	{
		if(!paused) camera.grabFrame();
		colorImg.setFromPixels(camera.getPixels(), width,height);
		colorImg.draw(0,0);
	}
	else
	{
		//float position = videoPlayer.getPosition();
		//if(position <= 0.1)
		//	videoPlayer.setPosition(0.4);
		
		videoPlayer.draw(0,0);
	}
	int nothing = 0;
	ofNotifyEvent(NEW_PIXELS,nothing,this);
}