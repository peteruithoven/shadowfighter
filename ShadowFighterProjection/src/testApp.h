#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"
#include "HitIndicator.h"
#include "ofxOsc.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
	
	private:
	
		int x;
		int y;
		float scale;
	
		ofxOscReceiver	receiver;
		void addHit(int hitX,int hitY);
		vector<DisplayObject*> children;
	
		bool ofKeyAlt() {
			return (glutGetModifiers() & GLUT_ACTIVE_ALT);
		}
		
		bool ofKeyShift() {
			return (glutGetModifiers() & GLUT_ACTIVE_SHIFT);
		}
		
		bool ofKeyControl() {
			return (glutGetModifiers() & GLUT_ACTIVE_CTRL);
		}
};

#endif
