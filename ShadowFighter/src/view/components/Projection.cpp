/*
 *  VisualFeedback.cpp
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 5/30/09.
 */

#include "Projection.h"

#define HOST			"localhost"
#define PORT			12345
#define HIT_ADDRESS		0
#define BOUNDING_BOX	1
#define HEALTH			2
#define WINNER			3
#define STATE			4
#define PLAYERS_UPDATE	5
#define COUNT_DOWN		6

Projection::Projection()
{
	sender.setup( HOST, PORT );
	ofAddListener(ofEvents.update, this, &Projection::update);
}
void Projection::addHit(int x,int y)
{
	//cout << "Projection::addHit: "<<x<<"x"<<y<<"\n";
	ofxOscMessage * message = new ofxOscMessage();
	message->setAddress(ofToString(HIT_ADDRESS));
	message->addIntArg(x);
	message->addIntArg(y);
	messagesBundle.addMessage(*message);
}
void Projection::updateHealth(float player1Health,float player2Health)
{
	ofxOscMessage * message = new ofxOscMessage();
	message->setAddress(ofToString(HEALTH));
	message->addFloatArg(player1Health);
	message->addFloatArg(player2Health);
	messagesBundle.addMessage(*message);
}
void Projection::updateState(int state)
{
	cout << "Projection::updateState: "<<state<<"\n";
	ofxOscMessage * message = new ofxOscMessage();
	message->setAddress(ofToString(STATE));
	message->addIntArg(state);
	messagesBundle.addMessage(*message);
}
void Projection::updateWinner(int winner)
{
	ofxOscMessage * message = new ofxOscMessage();
	message->setAddress(ofToString(WINNER));
	message->addIntArg(winner);
	messagesBundle.addMessage(*message);
}
void Projection::updatePlayers(bool detectedPlayer1, bool detectedPlayer2)
{
	ofxOscMessage * message = new ofxOscMessage();
	message->setAddress(ofToString(PLAYERS_UPDATE));
	message->addIntArg((detectedPlayer1)? 1 : 0);
	message->addIntArg((detectedPlayer2)? 1 : 0);
	messagesBundle.addMessage(*message);
}
void Projection::updateCountDown(int countDown)
{
	ofxOscMessage * message = new ofxOscMessage();
	message->setAddress(ofToString(COUNT_DOWN));
	message->addIntArg(countDown);
	messagesBundle.addMessage(*message);
}
void Projection::update(ofEventArgs & args)
{
	//cout << "Projection::update\n";
	//cout << "  model: " << &model << "\n";
	//cout << "  model->blobs->size(): " << model->blobs->size() << "\n";
	//if(model == NULL) return;
	
//	vector< vector<Blob*>* >*	blobsHistory = model->blobsHistory;
//	if(blobsHistory->size() < 1) return; 
//	vector<Blob*>*	prevBlobs = blobsHistory->at(0);
//	
//	for (int i = 0; i < prevBlobs->size(); i++)
//	{
//		ofxCvBlob * blob = prevBlobs->at(i);
//		ofRectangle blobRect = blob->boundingRect;
//		
//		ofxOscMessage * message = new ofxOscMessage();
//		message->setAddress(ofToString(BOUNDING_BOX));
//		message->addIntArg(blobRect.x);
//		message->addIntArg(blobRect.y);
//		message->addIntArg(blobRect.width);
//		message->addIntArg(blobRect.height);
//		messagesBundle.addMessage(*message);
//	}
	
	if(messagesBundle.getMessageCount() > 0)
	{
		sender.sendBundle(messagesBundle);
		messagesBundle.clear();
	}
}