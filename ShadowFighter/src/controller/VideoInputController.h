/*
 *  VideoInputController.h
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 4/13/10.
 *  Copyright 2010 HKU. All rights reserved.
 *
 */

#ifndef _VideoInputController
#define _VideoInputController

#include <ofUtils.h>
#include "BaseController.h"
#include "ofxCvGrayscaleImage.h"
#include "ofxCvContourFinder.h"

class VideoInputController : public BaseController
{
	public:
	
		VideoInputController();
		void newPixels(unsigned char * pixels);
	
	protected:
		
        ofxCvContourFinder	contourFinder;	
		ofxCvContourFinder	hitContourFinder;	
		void analyze(unsigned char * pixels);
		float getAutoThreshold(ofxCvGrayscaleImage * image);
	
		ofxCvGrayscaleImage* getBlobImage(ofxCvBlob * blob, ofxCvGrayscaleImage * orgImage);
		void setPixelsSubRegion(ofxCvImage * orgImage, ofxCvImage * targetImage,int x, int y,int width, int height, bool color);
		ofRectangle getIntersection(ofRectangle rect1, ofRectangle rect2);
		void drawRect(ofRectangle rect,int x, int y, int color, int a);
		void drawRect(ofRectangle rect,int x, int y, int color);
		void checkHit();
		void filterProjection();
};

#endif
