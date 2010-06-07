#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"
#include "HitIndicator.h"
#include "BlockIndicator.h"
#include "BlockingIndicator.h"
#include "ofxOsc.h"
#include "ofxXmlSettings.h"
#include "HealthBar.h"
#include "Constants.h"
#include "DisplayObjectContainer.h"
#include "Image.h"

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
	
		int		x;
		int		y;
		float	scale;
		int		alpha;
		bool	debug;
		bool	disableDraw;
		int		winner;
		bool	lockState;
		int		state;
		int		prevState;
		int		countDown;
		int		videoW;
		int		videoH;
		int		screenW;
		int		screenH;
		
		ofxOscReceiver			receiver;
		Image*					player1Img;
		Image*					player2Img;
		Image*					countDownImg;
		bool					detectedPlayer1;	
		bool					detectedPlayer2;	
		DisplayObjectContainer	images;
		HealthBar				healthBarPlayer1;
		HealthBar				healthBarPlayer2;
		ofImage					img;
		DisplayObjectContainer	effects;
		vector<ofRectangle*>	boundingBoxes;
		ofxXmlSettings			xml;
	
		void addHit(float hitX,float hitY, float hitW, float HitH, int hitType);
		void addBlock(float blockX,float blockY, float blockW, float blockH, float blockVictim);
		void showBlocking(float blockingX,float blockingY, float blockingW, float blockingH, int blockingVictim);
	
		bool ofKeyAlt() {
			return (glutGetModifiers() & GLUT_ACTIVE_ALT);
		}
		
		bool ofKeyShift() {
			return (glutGetModifiers() & GLUT_ACTIVE_SHIFT);
		}
		
		bool ofKeyControl() {
			return (glutGetModifiers() & GLUT_ACTIVE_CTRL);
		}
	
		void loadData();
		void parseXML();
		void storeValues();
		void setState(int state);
		void updatePlayerImages();
		void updateCountDown();
};

#endif
