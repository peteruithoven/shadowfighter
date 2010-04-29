/*
 *  Timer.h
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 11/17/09.
 *  Copyright 2009 HKU. All rights reserved.
 *	
 */

#ifndef _Timer
#define _Timer

#include "ofMain.h"

class Timer{

	public:
		Timer();
	
		bool getRunning();
		int getCount();
		int getInterval();	
		/**
		 How many times should the Timer run?
		   0: repeats continues
		   >0: repeats that many times
		 */
		void setInterval(int value);
		int getRepeatCount();	
		void setRepeatCount(int value);
	
	
		void start();
		void reset();
		void pause();
		
		ofEvent< int > TICK;
	
	protected:
		bool running;
		int interval; //milliseconds
		int repeatCount;
		int startTime;
		int count;
	
		void update(ofEventArgs & args);
		void tick();
};

#endif
