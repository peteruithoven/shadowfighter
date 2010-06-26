#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	//ofSetFrameRate(3);
	ofSetFrameRate(60);
	ofBackground(0,0,0);
	
	model;
	
//	try
//	{
		projectionView.setModel(&model);
//	}
//	catch(Poco::Exception& e)
//	{
//		std::string what = e.displayText();
//		printf("EXCEPTION: projectionView: %s\n", what.c_str());
//	}
	try
	{
		mainView.setModel(&model);
	}
	catch(Poco::Exception& e)
	{
		std::string what = e.displayText();
		printf("EXCEPTION: mainView: %s\n", what.c_str());
	}
	
	try
	{
		videoInputView.setModel(&model);
	}
	catch(Poco::Exception& e)
	{
		std::string what = e.displayText();
		printf("EXCEPTION: videoInputView: %s\n", what.c_str());
	}
	
	try
	{
		displayView.setModel(&model);
		//settingsView.setModel(&model);
	}
	catch(Poco::Exception& e)
	{
		std::string what = e.displayText();
		printf("EXCEPTION: displayView: %s\n", what.c_str());
	}
	
	try
	{
		keyboardView.setModel(&model);
	}
	catch(Poco::Exception& e)
	{
		std::string what = e.displayText();
		printf("EXCEPTION: keyboardView: %s\n", what.c_str());
	}
	
	try
	{
		visualFeedbackView.setModel(&model);
	}
	catch(Poco::Exception& e)
	{
		std::string what = e.displayText();
		printf("EXCEPTION: visualFeedbackView: %s\n", what.c_str());
	}
	
	try
	{
		gameInfoView.setModel(&model);
	}
	catch(Poco::Exception& e)
	{
		std::string what = e.displayText();
		printf("EXCEPTION: gameInfoView: %s\n", what.c_str());
	}
	
	model.loadData();
	model.start();
	
	cout << "testApp::setup finished\n";
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

