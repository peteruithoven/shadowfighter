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

class VideoInput{

	public:
		ofVideoPlayer videoPlayer; // public for performance increase of direct calls
	
		VideoInput();
		void start();
		void stop();
	
		ofEvent< int > NEW_PIXELS;
	
	protected:
		
		void draw(ofEventArgs & args);
};

#endif
