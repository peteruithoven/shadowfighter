#ifndef _DETECTION
#define _DETECTION

#include "ofEvents.h"
#include "ofMain.h"
//#include "ofAddons.h"
#include "ofxCvMain.h"
//#include "ofxFidMain.h"
#include "stdio.h"

class Detector : public ofSimpleApp{
	
public:
	
	ofVideoGrabber 		vidGrabber;
	ofxCvGrayscaleImage grayImage;
	ofxCvGrayscaleImage grayBg;
	ofxCvGrayscaleImage	grayDiff;
	ofxCvColorImage		colorImg;
	
	ofTexture			videoTexture;
	
	//ofxFiducialTracker	fidfinder;
	
	ofImage				transparency;
	
	int 				threshold;
	bool				bLearnBakground;
	bool				backgroundSubOn;
	int					scrW;
	int					scrH;
	int					imgX;
	int					imgYDest;
	int					imgXDest;
	float				rotationDest;
	int					imgY;
	int					imgXold;
	int					imgYold;
	float					rotOld;
	
	int					offsetX;
	int					offsetY;
	float				rotateMovie;
	float				size;
	int					toets;
	int					originalSize;
	float					fadeAlpha;
	
	ofVideoPlayer 		fingerMovie;
	bool                frameByframe;
	bool				setupApp;
	bool				debugApp;
	int					timeVisible;
	int					timeGlasses;
	bool				isPlaying;
	
	char timeString[255];
	int 			counter;
	int 			counterGlasses;
	
	int 				snapCounter;
	char 				snapString[255];
	ofImage 			img;
	ofTrueTypeFont		cooper;
	bool 				bSnapshot;		
	float 				phase;
	int					valTH;
	ofSerial	serial;
	char		bytesRead[3];
	char		bytesReadString[1];
	char		checkString[1];
	bool		isWearingGlasses;
	float		ardValue;
	//int counter;
	
	
	
	void setup();
	void update(ofEventArgs & args);
	//void draw();
	void captureScreen();
	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased();
	float getAutoThreshold(ofxCvGrayscaleImage * image);
	float getValue();
	
	
};

#endif

