/*
 *  Model.cpp
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 4/27/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "Model.h"s

Model::Model()
{
	cout << "Model::Model\n";
	videoW				= 640;
	videoH				= 460;
	threshold			= 30;
	willLearnBackground = false;
	minBlobSize			= 100;
	maxBlobSize			= videoH;
	
	grayImg = new ofxCvGrayscaleImage();
	grayEmptyImg = new ofxCvGrayscaleImage();
	grayDiffImg = new ofxCvGrayscaleImage();
	grayImg->allocate(videoW,videoH);
	grayEmptyImg->allocate(videoW,videoH);
	grayDiffImg->allocate(videoW,videoH);
	
	
	int emptyArg = 0;
	ofNotifyEvent(VALUES_UPDATED,emptyArg,this); 
	
	ofAddListener(ofEvents.update, this, &Model::update);
	ofAddListener(timer.TICK, this, &Model::onTick);
	
}

void Model::learnBackground()
{
	cout << "Model::learnBackground\n";
	willLearnBackground = true;
}
void Model::setThreshold(int newValue)
{
	cout << "Model::setThreshold:" << newValue << "\n";
	threshold = newValue;
	int emptyArg = 0;
	ofNotifyEvent(VALUES_UPDATED,emptyArg,this); 
}
void Model::update(ofEventArgs & args)
{ 
	
}
void Model::onTick(int  & count)
{
	
}
