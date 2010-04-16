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

class Projection : public DisplayObject 
{

	public:
		Projection();
		void addHit(int x,int y);
	
	private:
		ofxOscSender sender;
		ofxOscBundle messagesBundle;
		void update(ofEventArgs & args);
};

#endif
