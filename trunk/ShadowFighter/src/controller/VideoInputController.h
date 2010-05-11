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

#include "ofMain.h"
#include "BaseController.h"
#include "ofxCvGrayscaleImage.h"
#include "ofxCvContourFinder.h"
#include <algorithm>
#include <vector>
#include <iostream>
#include "Constants.h"

class VideoInputController : public BaseController
{
	public:
	
		VideoInputController();
		void newPixels(unsigned char * pixels);

	protected:
		ofTrueTypeFont		font;
        ofxCvContourFinder	contourFinder;	
		ofxCvContourFinder	hitContourFinder;	
	
		void analyze(unsigned char * pixels);
		void storeBackgroundImage(unsigned char * pixels);
		void correctEmptyImage();
		void filterProjection();
		void findHitBlobs();
		void findShadowBlobs();
		void storeShadowBlobs();
		void analyseHitBlobsSimple(unsigned char * colorPixels);
		void analyzeShadowsForPlayers();
		void analyseHitBlobs();
		bool hitIsUnique(ofRectangle blobRect);
		bool hitsBody(ofRectangle hitBlobRect);
		bool isCorrectColor(ofRectangle hitBlobRect, unsigned char * colorPixels);
		void storeHistory();
		void storeBlobHistory();
		
		// debug
		void drawBlobsHistory();
		void drawHitText(string text);
		void drawAttackText(string text);
	
		// utils
		unsigned char * toGrayscale(unsigned char * pixels);
		void drawRect(ofRectangle rect,int x, int y, int color, int a);
		void drawRect(ofRectangle rect,int x, int y, int color);
		void sortBlobs(vector<Blob*> * blobs);
		void takeScreenShot(string extraText);
		void takeHitScreenShot(string extraText);
		bool matchBlobs(ofxCvBlob * blob1, ofxCvBlob * blob2);
		bool rectHitTest(ofRectangle rect1,ofRectangle rect2);
		void colorInImage(ofxCvGrayscaleImage * image, int color, ofRectangle rect);
		void colorDiff(ofxCvGrayscaleImage * diffImg, ofxCvColorImage * img1, ofxCvColorImage * img2);
};

#endif