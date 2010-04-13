#include "Detection.h"

//--------------------------------------------------------------
void Detector::setup(){	
	cout << "Detection instantiated\n";
	counter = 0;
	//ofSetFrameRate(10);
	// Overall Setup
	// ofBackground(0,0,0);
	setupApp=true;
	debugApp=true;
	counter = 0;
	scrH = 480;
	scrW = 640;
	valTH = 0;
	
	// Serial setup
	//ofSetVerticalSync(true);
	//serial.enumerateDevices();
	//serial.setup("/dev/tty.usbserial-A9007L0E", 9600);
	
	
	// Movie Setup
	vidGrabber.videoSettings();
	vidGrabber.listDevices();
	//vidGrabber.setDeviceID(1);
	
	
	vidGrabber.setVerbose(true);
	vidGrabber.initGrabber(scrW,scrH);
	colorImg.allocate(scrW,scrH);
	grayImage.allocate(scrW,scrH);
	grayBg.allocate(scrW,scrH);
	grayDiff.allocate(scrW,scrH);
	
	
	
	videoTexture.allocate(scrW,scrH, GL_RGBA);
	
	// initial state
	threshold = 80;
	bLearnBakground = false;
	backgroundSubOn = false;
	ofAddListener(ofEvents.update, this, &Detection::update);
}

//--------------------------------------------------------------
void Detection::update(ofEventArgs & args){
	
	//cout << "DetectionUpdated\n";
	vidGrabber.grabFrame();
	
	if (vidGrabber.isFrameNew()){
		grayBg = grayImage;
		colorImg.setFromPixels(vidGrabber.getPixels(), scrW,scrH);
		grayImage = colorImg;
		
		if (bLearnBakground == true){
			grayBg = grayImage;		// the = sign copys the pixels from grayImage into grayBg (operator overloading)
			bLearnBakground = false;
			backgroundSubOn = true;
		}
		
		// take the abs value of the difference between background and incoming and then threshold:
		if (!backgroundSubOn) {
			grayDiff.absDiff( grayBg, grayImage );
		} else {
			grayDiff = grayImage;
		}
		
		grayDiff.threshold(getAutoThreshold(&grayImage));
		
	}
	
	int tcounter = 0;
	ofSetColor(0xFFFFFF);
	unsigned char * pixels = grayDiff.getPixels();
    // let's move through the "RGB" char array
    // using the red pixel to control the size of a circle.
    for (int i = 0; i < scrW; i+=1){
        for (int j = 0; j < scrH; j+=1){
            unsigned char r = pixels[(j * 320 + i)*1];
            float val = 1 - ((float)r / 255.0f);
			// ofCircle(800 + i,1+j,1*val);
			if(r == 0){
				tcounter++;	
			}
			
        }
    }
	counter = tcounter;
	
	
}

//--------------------------------------------------------------
/*void Detection::draw(){
	
	if(debugApp){
		grayDiff.draw(360,0,320,240);
		colorImg.draw(360,360,320,240);
	}
	
	if(setupApp){
		ofSetColor(0,0,0);
		ofRect(0, 0, 369, 240);
		ofSetColor(255,255,255);
		ofDrawBitmapString( "Threshold: ", 2,15);
		ofDrawBitmapString( "change the threshold by pressing '-' or '+'", 2,30);
		ofDrawBitmapString( "Arduino: ", 2,45);
		ofDrawBitmapString( "Detection: ", 2,60);
		ofDrawBitmapString( "value: ", 2,75);
		ofDrawBitmapString( ofToString( getAutoThreshold(&grayImage)), 90,15);
		ofDrawBitmapString( bytesReadString, 90,45);
		ofDrawBitmapString( ofToString(counter), 90,60);
		
		int maxV = scrH * scrW;
		ardValue = ( maxV/(float)counter)-1;
		
		
		
		ofDrawBitmapString( ofToString(ardValue), 90,75);
	}
	
	
	
	
	
	
	
}*/

//--------------------------------------------------------------

void Detection::keyPressed  (int key){ 
	//toets = key;
	if( key == '-' ) {
        threshold = MAX( 0, threshold-1 );
    } else if( key == '+' || key == '=' ) {
        threshold = MIN( 255, threshold+1 );
    } else if( key == 'b' ) {
		backgroundSubOn = false;
	}else if (key == 's' || key == 'S'){
		if(setupApp){
			setupApp = false;	
		}else{
			setupApp = true;
		}
	}
	
	
	else if (key == 'd' || key == 'D'){
		if(debugApp){
			debugApp = false;
			setupApp = false;
		}else{
			debugApp = true;
			setupApp = true;
		}
	}else if (key == 'r' || key == 'R'){
		bLearnBakground = true;
		backgroundSubOn = false;
	}
	
}
//--------------------------------------------------------------
float Detection::getValue  (){ 
	
	int maxV = scrH * scrW;

	ardValue = (( maxV/(float)counter)-1) * 2;
	cout << "screenPixelSize: " + ofToString(maxV) << endl;
	return ardValue;
}

//--------------------------------------------------------------
void Detection::keyReleased  (int key){ 
	
}

//--------------------------------------------------------------
void Detection::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void Detection::mouseDragged(int x, int y, int button){
}

//--------------------------------------------------------------
void Detection::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void Detection::mouseReleased(){
	
}


float Detection::getAutoThreshold(ofxCvGrayscaleImage * image)
{
    //cout << "\nObjectTracker::getAutoTreshol";
    unsigned char * pixels = image->getPixels();
    int length = image->getWidth()*image->getHeight();
	
    int minLum = 255;
    int maxLum = 0;
    for (int i = 0; i < length; i++) {
        unsigned char pixel = pixels[i];       
        int lum = pixel;
		
        if(lum<minLum)
            minLum = lum;
        if(lum>maxLum)
            maxLum = lum;
    }
    int threshold = (minLum+maxLum)/2-65;
  //  cout << "\n    threshold: ";
  //  cout << threshold;
    return threshold;
}
