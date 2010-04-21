/*
 *  KeyboardController.cpp
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 5/8/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "KeyboardController.h"

KeyboardController::KeyboardController()
{
	
}

void KeyboardController::keyPressed(int keyCode)
{
	switch (keyCode){
		case ' ':
			model->learnBackground();
			break;
		case '+':
		case '=':
			model->setThreshold(model->threshold+1);
			break;
		case '-':
		case '_':
			model->setThreshold(model->threshold-1);
			break;
		case ']':
			model->setHitThreshold(model->hitThreshold+1);
			break;
		case '[':
			model->setHitThreshold(model->hitThreshold-1);
			break;
		case 'd':
			model->debugDetection = !model->debugDetection;
			break;
			
			
		/*case 'i':
			model->setInvert(!model->getInvert());
			break;
		case ',':
			model->setObjectsScale(model->getObjectsScale()-0.1);
			break;
		case '.':
			model->setObjectsScale(model->getObjectsScale()+0.1);
			break;
		*/
	};
}
void KeyboardController::keyReleased(int keyCode)
{
	
}