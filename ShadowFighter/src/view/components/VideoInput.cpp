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
	cout << "VideoInput::VideoInput\n";
	ofAddListener(ofEvents.draw, this, &VideoInput::draw);
	videoPlayer.loadMovie("movies/clip1.mov");
	start();
}
void VideoInput::start()
{
	cout << "VideoInput::start\n";
	videoPlayer.play();
}
void VideoInput::stop()
{
	videoPlayer.stop();
}
void VideoInput::draw(ofEventArgs & args)
{ 
	cout << "VideoInput::draw\n";
	videoPlayer.draw(10.0,10.0);
	
	int nothing = 0;
	ofNotifyEvent(NEW_PIXELS,nothing,this);
}