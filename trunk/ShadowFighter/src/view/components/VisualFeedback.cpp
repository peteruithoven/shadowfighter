/*
 *  VisualFeedback.cpp
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 5/30/09.
 */

#include "VisualFeedback.h"
#include "HitIndicator.h"

VisualFeedback::VisualFeedback()
{
	children = *new vector<DisplayObject*>;
	ofAddListener(ofEvents.draw, this, &VisualFeedback::draw);
}
void VisualFeedback::addHit(int x,int y)
{
	HitIndicator *hitIndicator = new HitIndicator();
	hitIndicator->x = x;
	hitIndicator->y = y;
	hitIndicator->start();
	children.push_back(hitIndicator);
}

void VisualFeedback::draw(ofEventArgs & args)
{
	for(int i = 0;i<children.size();i++)
	{
		DisplayObject* displayObject = children.at(i);
		HitIndicator* hitIndicator = (HitIndicator*)displayObject;
		
		hitIndicator->draw();
		if(!hitIndicator->running)
		{
			children.erase(children.begin()+i);
			i--;
		}
	}
}