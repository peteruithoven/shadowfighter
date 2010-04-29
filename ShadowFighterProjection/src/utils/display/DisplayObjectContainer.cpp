/*
 *  DisplayObjectContainer.cpp
 *  ShadowFighterProjection
 *
 *  Created by Peter Uithoven on 4/28/10.
 */

#import "DisplayObjectContainer.h"


DisplayObjectContainer::DisplayObjectContainer()
{
	children = *new vector<DisplayObject*>;
	
	ofAddListener(ofEvents.draw, this, &DisplayObjectContainer::draw);
}

void DisplayObjectContainer::addChild(DisplayObject * child)
{
	children.push_back(child);
}
void DisplayObjectContainer::removeChildAt(int index)
{
	children.erase(children.begin()+index);
}
void DisplayObjectContainer::clear()
{
	setAutoDraw(false);
	children.clear();
}
int DisplayObjectContainer::size()
{
	return children.size();
}
DisplayObject * DisplayObjectContainer::getChildAt(int index)
{
	return children.at(index);
}

void DisplayObjectContainer::draw(ofEventArgs & args)
{ 
	for(int i = 0;i<children.size();i++)
	{
		DisplayObject* displayObject = children.at(i);
		//displayObject->draw();
	}
}

void DisplayObjectContainer::setAutoDraw(bool newValue)
{
	autoDraw = newValue;
	for(int i = 0;i<children.size();i++)
	{
		DisplayObject* displayObject = children.at(i);
		displayObject->setAutoDraw(newValue);
	}
}