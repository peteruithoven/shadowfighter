#include "testApp.h"

#define HOST "localhost"
#define PORT 12345
#define HIT_ADDRESS 0

//--------------------------------------------------------------
void testApp::setup()
{
	ofSetFrameRate(31);
	ofBackground(0,0,0);
	ofSetCircleResolution(100);
	children = *new vector<DisplayObject*>;
	x = 0;
	y = 0;
	scale = 1;
	
	receiver.setup(PORT);
}

void testApp::addHit(int hitX,int hitY)
{
	HitIndicator *hitIndicator = new HitIndicator();
	hitIndicator->x = x+hitX*scale;
	hitIndicator->y = y+hitY*scale;
	hitIndicator->scale = scale;
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
void testApp::keyPressed(int keyCode)
{
}

//--------------------------------------------------------------
void testApp::keyReleased(int keyCode)
{
	float step;
	if(ofKeyShift()) step = 10;
	else if(ofKeyControl()) step = 1;
	else step = 5;
	
	switch (keyCode){
		case ' ':
			ofToggleFullscreen();
			break;
		case '+':
		case '=':
			scale += step/100;
			break;
		case '-':
		case '_':
			scale -= step/100;
			break;
		case OF_KEY_UP:
			y -= step;
			break;
		case OF_KEY_DOWN:
			y += step;
			break;
		case OF_KEY_LEFT:
			x -= step;
			break;
		case OF_KEY_RIGHT:
			x += step;
			break;
	}
	
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


