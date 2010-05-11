/*
 *  Image.cpp
 *  ShadowFighterProjection
 *
 *  Created by Peter Uithoven on 4/28/10.
 */

#import "Image.h"


Image::Image()
{
	
}
void Image::draw()
{ 
	//cout << "Image::draw\n";
	if(img.getWidth() > 0 && img.getHeight() > 0)
	{
		if(img.type == OF_IMAGE_COLOR_ALPHA)
			ofEnableAlphaBlending();
		ofSetColor(0xffffff);
		img.draw(x,y);
		if(img.type == OF_IMAGE_COLOR_ALPHA)
			ofDisableAlphaBlending();
	}
}
void Image::destroy()
{
	img.clear();
}