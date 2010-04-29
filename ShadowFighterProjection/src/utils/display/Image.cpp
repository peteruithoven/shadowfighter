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
	if(img.getWidth() > 0 && img.getHeight() > 0)
	{
		if(img.type == OF_IMAGE_COLOR_ALPHA)
			ofEnableAlphaBlending();
		img.draw(x,y);
		if(img.type == OF_IMAGE_COLOR_ALPHA)
			ofDisableAlphaBlending();
	}
}