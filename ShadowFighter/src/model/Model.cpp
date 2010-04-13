/*
 *  Model.cpp
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 4/27/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "Model.h"
#include <numeric>

Model::Model()
{
	ofAddListener(ofEvents.update, this, &Model::update);
	ofAddListener(timer.TICK, this, &Model::onTick);
}

void Model::update(ofEventArgs & args)
{ 
	
}
void Model::onTick(int  & count)
{
	
}
