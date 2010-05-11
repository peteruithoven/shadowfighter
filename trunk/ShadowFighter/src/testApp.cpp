#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	//ofSetFrameRate(3);
	ofSetFrameRate(60);
	ofBackground(0,0,0);
	
	model;
	projectionView.setModel(&model);
	mainView.setModel(&model);
	videoInputView.setModel(&model);
	displayView.setModel(&model);
	//settingsView.setModel(&model);
	keyboardView.setModel(&model);
	visualFeedbackView.setModel(&model);
	gameInfoView.setModel(&model);
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
