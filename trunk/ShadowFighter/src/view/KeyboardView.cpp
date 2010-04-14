/*
 *  KeyboardView.cpp
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 5/8/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "KeyboardView.h"

KeyboardView::KeyboardView()
{
	ofAddListener(ofEvents.keyReleased, this, &KeyboardView::keyReleased);
	ofAddListener(ofEvents.keyPressed, this, &KeyboardView::keyPressed);
}
void KeyboardView::setModel(Model * model)
{
	controller.setModel(model);
}
void KeyboardView::keyReleased(ofKeyEventArgs & args)
{
	controller.keyReleased(args.key);
};
void KeyboardView::keyPressed(ofKeyEventArgs & args)
{
	controller.keyPressed(args.key);
}