/*
 *  Effect.h
 *  ShadowFighterProjection
 *
 *  Created by Peter Uithoven on 5/25/10.
 */

#ifndef _Effect
#define _Effect

#include "ofMain.h"
#include "ofEvents.h"
#include "DisplayObject.h"

class Effect : public DisplayObject 
{
	public:
		bool running;

		Effect();
		virtual void start();
		void stop();
	
	private:
};

#endif