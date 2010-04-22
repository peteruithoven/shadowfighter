/*
 *  VisualFeedback.h
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 5/30/09.
 *
 */

#ifndef _Projection
#define _Projection

#include "ofMain.h"
#include "ofxOsc.h"
#include "DisplayObject.h"
#include "Model.h"

class Projection : public DisplayObject 
{

	public:
	
		Model * model;
	
		Projection();
		void addHit(int x,int y);
	
	private:
		ofxOscSender sender;
		ofxOscBundle messagesBundle;
		void update(ofEventArgs & args);
};

#endif