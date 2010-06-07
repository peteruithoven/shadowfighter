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
	mirrorX = false;
	 
	setAutoDraw(false);
	
	bgImg.loadImage("images/healthbar/healthbar_left_bg.png");
	barImgOrg.loadImage("images/healthbar/healthbar_left_fg.png");
	height = bgImg.height;
	width = bgImg.width;
}

void HealthBar::draw()
{
	//cout << "  HealthBar::draw\n";
//	ofNoFill();
//	ofSetColor(0xffffff);
//	ofSetLineWidth(2);
//	ofRect(x, y, width, height);
//	ofSetLineWidth(1);
//	
//	ofFill();
//	ofRect(x, y, width*percentage, height);

	//cout << "percentage: " << percentage << "\n";
	
	ofImage barImg = barImgOrg;
	unsigned char *pixels = barImg.getPixels();
	
	int borderWidthLeft = 5;
	int borderWidthRight = 9;

	int barWidth = (width-borderWidthLeft-borderWidthRight)*percentage;
	int beginX = borderWidthLeft; //(mirrorX)? borderWidth : barWidth;
	int endX = width-barWidth-borderWidthRight; //(mirrorX)? width-barWidth : width;
	
	for (int ix = beginX; ix < endX; ix++) 
	{
		for (int iy = 0; iy < height; iy++) 
		{
			int pixelIndex = ( (iy*width) + ix ) *4;
			pixels[pixelIndex+3] = 0; //alpha
		}
	}
	
	barImg.setFromPixels(pixels, width, height, OF_IMAGE_COLOR_ALPHA, true);
	
	ofEnableAlphaBlending();
	ofSetColor(255,255,255,255);
	
	ofPushMatrix();
	ofTranslate(x, y, 0);
	float scaleX = (mirrorX)? -1 : 1;
	ofScale(scaleX, 1, 1);
	
	int imgX = (mirrorX)? -bgImg.getWidth() : 0;
	
	bgImg.draw(imgX, 0);
	barImg.draw(imgX, 0);
	ofPopMatrix();
	
	ofDisableAlphaBlending();
}
