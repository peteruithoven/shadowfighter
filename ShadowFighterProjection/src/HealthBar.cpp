/*
 *  HealthBar.cpp
 *  ShadowFighter
 *
 *  Created by Peter Uithoven on 4/24/10.
 */

#import "HealthBar.h"


HealthBar::HealthBar()
{
	percentage = 1;
	width = 150;
	height = 30;
	 
	setAutoDraw(false);
	
	bgImg.loadImage("images/healthbar/healthbar_bg_red.png");
	barImgOrg.loadImage("images/healthbar/healthbar_fg_yellow.png");
	height = bgImg.height;
	width = bgImg.width;
}

void HealthBar::draw()
{
//	ofNoFill();
//	ofSetColor(0xffffff);
//	ofSetLineWidth(2);
//	ofRect(x, y, width, height);
//	ofSetLineWidth(1);
//	
//	ofFill();
//	ofRect(x, y, width*percentage, height);
	
	unsigned char *pixels = barImgOrg.getPixels();
	
	int borderWidth = 7;
	int barWidth = (width-borderWidth*2)*percentage + borderWidth;
	
	for (int ix = barWidth; ix < width; ix++) 
	{
		for (int iy = 0; iy < height; iy++) 
		{
			int pixelIndex = ( (iy*width) + ix ) *4;
			pixels[pixelIndex+3] = 0; //a
		}
	}
	ofImage barImg;
	barImg.setFromPixels(pixels, width, height, OF_IMAGE_COLOR_ALPHA, true);
	ofEnableAlphaBlending();
	bgImg.draw(x, y);
	barImg.draw(x, y);
	ofDisableAlphaBlending();
	
}
