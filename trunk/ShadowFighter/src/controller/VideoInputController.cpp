/*
 *  VideoInputController.cpp
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 4/13/10.
 *  Copyright 2010 HKU. All rights reserved.
 *
 */

#include "VideoInputController.h"

VideoInputController::VideoInputController()
{
}
void VideoInputController::newPixels(unsigned char * pixels)
{
	// analyze pixels, using configuration stored in model
	
	int videoW = model->videoW;
	int videoH = model->videoH;
	
	// convert color image to rgb
	unsigned char * grayPixels = new unsigned char[videoW*videoH];
	int counter = 0;
	for (int i = 0; i < videoH; i++) {
		for (int j = 0; j < videoW*3; j+=3) {
			
			// get r, g and b components
			int r = (i*videoW*3) + j;
			int g = (i*videoW*3) + (j+1);
			int b = (i*videoW*3) + (j+2);
			
			int grayPixel = (11 * pixels[r] + 16 * pixels[g] + 5 * pixels[b]) / 32;
			
			grayPixels[counter] = grayPixel;
			
			counter++;
		}
	}
	
	model->grayImg->setFromPixels(grayPixels, videoW,videoH);
	//model->grayImg->draw(videoW, 0);
	if (model->willLearnBackground)
	{
		cout << "VideoInputController::newPixels willLearnBackground\n";
		model->grayEmptyImg->setFromPixels(grayPixels,videoW,videoH);
		//model->grayEmptyImg->saveImage("empty.png");
		model->willLearnBackground = false;
		
		model->grayEmptyImg->draw(videoW, 0);
	}
	
	//take the abs value of the difference between background and incoming and then threshold:
	model->grayDiffImg->absDiff(*model->grayEmptyImg, *model->grayImg);
	model->grayDiffImg->draw(0, videoH);
	
	model->grayDiffImg->threshold(model->threshold);
	model->grayDiffImg->draw(videoW, videoH);
	
	contourFinder.findContours(*model->grayDiffImg, model->minBlobSize, model->maxBlobSize, model->maxNumBlobs, true);
	
	float blobDisplayX = videoW;
	float blobDisplayY = videoH;
	int numBlobs = contourFinder.nBlobs;
    for (int i = 0; i < numBlobs; i++)
	{
		ofxCvBlob blob = contourFinder.blobs[i];
		
		float blobX = blob.boundingRect.x;
		float blobY = blob.boundingRect.y;
		float blobWidth = blob.boundingRect.width;
		float blobHeight = blob.boundingRect.height;
		
		/*if(!(blobX > activeAreaX &&
			 blobY > activeAreaY &&
			 blobX+blobWidth < activeAreaX+activeAreaWidth &&
			 blobY+blobHeight < activeAreaY+activeAreaHeight))
			continue;*/
		
		blob.draw(blobDisplayX,blobDisplayY);
	}			
}

/*float VideoInputController::getAutoThreshold(ofxCvGrayscaleImage * image)
{
    //cout << "\VideoInputController::getAutoThreshold";
    unsigned char * pixels = image->getPixels();
    int length = image->getWidth()*image->getHeight();
	
    int minLum = 255;
    int maxLum = 0;
    for (int i = 0; i < length; i++) {
        unsigned char pixel = pixels[i];       
        int lum = pixel;
        if(lum<minLum)
            minLum = lum;
        if(lum>maxLum)
            maxLum = lum;
    }
    int threshold = (minLum+maxLum)/2;
	//  cout << "\n    threshold: ";
	//  cout << threshold;
    return threshold;
}*/