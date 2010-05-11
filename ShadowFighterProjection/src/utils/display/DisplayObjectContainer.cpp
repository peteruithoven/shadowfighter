/*
 *  DisplayObjectContainer.cpp
 *  
 *
 *  Created by Peter Uithoven on 4/28/10.
 */

#import "DisplayObjectContainer.h"


DisplayObjectContainer::DisplayObjectContainer()
{
	children = *new vector<DisplayObject*>;
	
	setAutoDraw(false);
}
DisplayObject* DisplayObjectContainer::addChild(DisplayObject * child)
{
	children.push_back(child);
	return child;
}
void DisplayObjectContainer::removeChildAt(int index)
{
	DisplayObject* displayObject = children.at(index);
	displayObject->destroy();
	delete displayObject;
	
	children.erase(children.begin()+index);
}
void DisplayObjectContainer::clear()
{
	for(int i = 0;i<children.size();i++)
	{
		DisplayObject* displayObject = children.at(i);
		displayObject->destroy();
		delete displayObject;
	}	
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
void DisplayObjectContainer::draw()
{ 
	for(int i = 0;i<children.size();i++)
	{
		DisplayObject* displayObject = children.at(i);
		displayObject->draw();
	}
}