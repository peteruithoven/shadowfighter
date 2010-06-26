/*
 *  BlockIndicator.h
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 5/30/09.
 */

#ifndef _BlockIndicator
#define _BlockIndicator

#include "ofMain.h"
#include "Effect.h"

class BlockIndicator : public Effect 
{

	public:
		int						alphaChange;
		float					scale;
		bool					mirrorX;
		int						imgX;
		ofImage					img;
	
		BlockIndicator();
		void start();
		void draw();
	private:
		
};

#endif
