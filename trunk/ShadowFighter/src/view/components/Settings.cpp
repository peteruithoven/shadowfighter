/*
 *  Settings.cpp
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 11/19/09.
 *  Copyright 2009 HKU. All rights reserved.
 *
 */

#include "Settings.h"

Settings::Settings()
{
	
	
}

void Settings::setup()
{
	gui.setAutoSave(true);
	//gui.addSlider("flowIntervalRatio",&flowIntervalRatio,2000,8000);
	//gui.addSlider("baseInterval",&baseInterval,0,10000);
	
	/*gui.addToggle("useManualFlowValue", model->useManualFlowValue);
	gui.addSlider("manualFlowValue",model->manualFlowValue,0,1);
	gui.addSlider("Num averaging frames",model->numAveragingFrames,1,100);
	gui.addSlider("FlowValue multiplier",model->flowValueMultiplier,1,5);
	gui.addSlider("RO: flowValue", model->flowValue, 0,1);
	gui.addSlider("flowIntervalRatio",model->flowIntervalRatio,2000,8000);
	gui.addSlider("baseInterval",model->baseInterval,0,10000);
	gui.addSlider("max decrease percentage",model->maxDecreasePercentage,0,0.05);
	gui.addSlider("RO: interval", model->interval, 0,10000);
	gui.loadFromXML();*/
	
	ofAddListener(ofEvents.draw, this, &Settings::draw);
}


void Settings::draw(ofEventArgs & args)
{
	//gui.draw();
}
