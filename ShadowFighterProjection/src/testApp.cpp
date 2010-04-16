#include "testApp.h"

#define HOST "localhost"
#define PORT 12345
#define HIT_ADDRESS 0

//--------------------------------------------------------------
void testApp::setup()
{
	ofSetFrameRate(31);
	ofBackground(0,0,0);
	
	receiver.setup(PORT);
	
	children = *new vector<DisplayObject*>;
}

void testApp::addHit(int x,int y)
{
	HitIndicator *hitIndicator = new HitIndicator();
	hitIndicator->x = x;
	hitIndicator->y = y;
	hitIndicator->start();
	children.push_back(hitIndicator);
}




//--------------------------------------------------------------
void testApp::update()
{
	ofxOscMessage m;
	while(receiver.getNextMessage(&m))
	{
		switch (ofToInt(m.getAddress()))
		{
			case HIT_ADDRESS:
				int hitX = m.getArgAsInt32(0);
				int hitY = m.getArgAsInt32(1);
				addHit(hitX,hitY);
				break;
		}
		m.clear();
	}
	for(int i = 0;i<children.size();i++)
	{
		DisplayObject* displayObject = children.at(i);
		HitIndicator* hitIndicator = (HitIndicator*)displayObject;
		if(!hitIndicator->running)
		{
			children.erase(children.begin()+i);
			i--;
		}
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	for(int i = 0;i<children.size();i++)
	{
		DisplayObject* displayObject = children.at(i);
		HitIndicator* hitIndicator = (HitIndicator*)displayObject;
		hitIndicator->draw();
	}
}

//--------------------------------------------------------------
void testApp::keyPressed(int key)
{
	
}

//--------------------------------------------------------------
void testApp::keyReleased(int key)
{
	ofToggleFullscreen();
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y )
{

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{
	ofToggleFullscreen();
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{

}


