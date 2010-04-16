#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "Model.h"
#include "MainView.h"
#include "DisplayView.h"
#include "SettingsView.h"
#include "VideoInputView.h"
#include "keyboardView.h"
#include "VisualFeedbackView.h"
#include "ProjectionView.h"

class testApp : public ofBaseApp{

	public:
	
		Model model;
		ProjectionView projectionView;
		VideoInputView videoInputView;
		MainView mainView;
		DisplayView displayView;
		//SettingsView settingsView;
		KeyboardView keyboardView;
		VisualFeedbackView visualFeedbackView;
		
		
	
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

};

#endif
