#include "testApp.h"

#define HOST "localhost"
#define PORT 12345
#define HIT_ADDRESS 0
#define BOUNDING_BOX 1

//--------------------------------------------------------------
void testApp::setup()
{
	videoW				= 640;
	videoH				= 460;
	screenW				= 1024;
	screenH				= 768;
	
	ofSetFrameRate(31);
	ofBackground(0,0,0);
	ofSetCircleResolution(100);
	
	children = *new vector<DisplayObject*>;

	loadData();
	
	receiver.setup(PORT);
}

void testApp::loadData()
{
	if( xml.loadFile("config.xml") ){
		cout << "config.xml loaded\n";
	}else{
		cout << "Unable to load config.xml check data folder\n";
	}
	parseXML();
	storeValues();
}
void testApp::parseXML()
{
	x				= xml.getValue("x", 1.0);
	y				= xml.getValue("y", 1.0);
	alpha			= xml.getValue("alpha", 1.0);
	scale			= xml.getValue("scale", 1.00);
	debug			= xml.getValue("debug", false);
	
	cout << "x: " << x << "\n";
	cout << "y: " << y << "\n";
	cout << "scale: " << scale << "\n";
	cout << "alpha: " << alpha << "\n";
	cout << "debug: " << debug << "\n";
}
void testApp::storeValues()
{
	xml.setValue("x", x);
	xml.setValue("y", y);
	xml.setValue("scale", scale);
	xml.setValue("alpha", alpha);
	xml.setValue("debug", debug);
	xml.saveFile("config.xml");
}


void testApp::addHit(float hitX,float hitY)
{
	HitIndicator *hitIndicator = new HitIndicator();
	hitIndicator->x = hitX/videoW*screenW*scale+x;
	hitIndicator->y = hitY/videoH*screenH*scale+y;
	hitIndicator->scale = scale;
	hitIndicator->alpha = alpha;
	hitIndicator->start();
	children.push_back(hitIndicator);
}




//--------------------------------------------------------------
void testApp::update()
{
	//cout << "testApp::update\n";
	boundingBoxes.clear();
	ofxOscMessage m;
	while(receiver.getNextMessage(&m))
	{
		cout << "  m.getAddress(): " << m.getAddress() << "\n";
		switch (ofToInt(m.getAddress()))
		{
			case HIT_ADDRESS:
				{
				int hitX = m.getArgAsInt32(0);
				int hitY = m.getArgAsInt32(1);
				addHit(hitX,hitY);
				}
				break;
			case BOUNDING_BOX:
				if(debug)
				{
					ofRectangle * boundingBox = new ofRectangle();						
					boundingBox->x = float(m.getArgAsInt32(0))/videoW*screenW*scale+x;
					boundingBox->y = float(m.getArgAsInt32(1))/videoH*screenH*scale+y;
					boundingBox->width = float(m.getArgAsInt32(2))/videoW*screenW*scale;
					boundingBox->height = float(m.getArgAsInt32(3))/videoH*screenH*scale;
					
					boundingBoxes.push_back(boundingBox);
					
				}
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
void testApp::draw()
{
	//cout << "testApp::draw\n";
	
	for(int i = 0;i<children.size();i++)
	{
		DisplayObject* displayObject = children.at(i);
		HitIndicator* hitIndicator = (HitIndicator*)displayObject;
		hitIndicator->draw();
	}
	
	if(debug)
	{
		ofEnableAlphaBlending();
		ofSetColor(255,255,255,alpha);
		//ofSetColor(255,255,255);
		
		//cout << "  boundingBoxes.size(): " << boundingBoxes.size() << "\n";
		
		for(int i = 0;i<boundingBoxes.size();i++)
		{
			ofRectangle* boundingBox = boundingBoxes.at(i);
			ofNoFill();
			ofSetLineWidth(5);
			ofRect(boundingBox->x,boundingBox->y,boundingBox->width,boundingBox->height);
		}
		
		ofNoFill();
		ofSetLineWidth(20);
		ofRect(0,0,1024,768);
		
		ofFill();
		ofEllipse(1024/2,768/2,50,50);
		ofDisableAlphaBlending();
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
		case ',':
			alpha -= step;
			break;
		case '.':
			alpha += step;
			break;
		case 'd':
			debug = !debug;
			break;
	}
	cout << "x: " << x << " y: " << y << " scale: " << scale << "\n";
	storeValues();
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
	//ofToggleFullscreen();
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{

}


