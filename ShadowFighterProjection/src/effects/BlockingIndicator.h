/*
 *  BlockingIndicator.h
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 5/30/09.
 */

#ifndef _BlockingIndicator
#define _BlockingIndicator

#include "ofMain.h"
#include "Effect.h"

class BlockingIndicator : public Effect 
{

	public:
		int						alphaChange;
		float					scale;
		bool					mirrorX;
		ofImage					img;
		int						imgX;
		BlockingIndicator();
		void start();
		void draw();
	private:
		
};

#endif
