/*
 *  VisualFeedback.h
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 5/30/09.
 *
 */

#ifndef _VisualFeedback
#define _VisualFeedback

#include <ofUtils.h>
#include "ofEvents.h"
#include "DisplayObject.h"

class VisualFeedback : public DisplayObject 
{

	public:
		VisualFeedback();
		void addHit(int x,int y);
	
	private:
		vector<DisplayObject*> children;
		void draw(ofEventArgs & args);
};

#endif
