/*
 *  VideoInput.h
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 4/12/10.
 *  Copyright 2010 HKU. All rights reserved.
 *
 */

#ifndef _VideoInput
#define _VideoInput

#include <ofUtils.h>
#include <ofVideoPlayer.h>
#include "ofEvents.h"
#include "DisplayObject.h"
#include "ofxCvColorImage.h"


class VideoInput : public DisplayObject{

	public:
		ofVideoPlayer	videoPlayer; // public for performance increase of direct calls
		ofVideoGrabber	camera;
		bool			useCamera;
		VideoInput();
		void init(bool useCamera,string movieURL);
		void start();
		void stop();
	
		ofEvent< int > NEW_PIXELS;
	

	
	protected:
		bool	running;
		ofxCvColorImage colorImg;
		void draw(ofEventArgs & args);
};

#endif
