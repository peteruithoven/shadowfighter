/*
 *  VisualFeedback.cpp
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 5/30/09.
 */

#include "Projection.h"

#define HOST "localhost"
#define PORT 12345
#define HIT_ADDRESS 0
#define BOUNDING_BOX 1

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
void Projection::update(ofEventArgs & args)
{
	//cout << "Projection::update\n";
	//cout << "  model: " << &model << "\n";
	//cout << "  model->blobs->size(): " << model->blobs->size() << "\n";
	//if(model == NULL) return;
	for (int i = 0; i < model->blobs->size(); i++)
	{
		ofxCvBlob * blob = model->blobs->at(i);
		ofRectangle blobRect = blob->boundingRect;
		
		ofxOscMessage * message = new ofxOscMessage();
		message->setAddress(ofToString(BOUNDING_BOX));
		message->addIntArg(blobRect.x);
		message->addIntArg(blobRect.y);
		message->addIntArg(blobRect.width);
		message->addIntArg(blobRect.height);
		messagesBundle.addMessage(*message);
	}
	
	if(messagesBundle.getMessageCount() > 0)
	{
		sender.sendBundle(messagesBundle);
		messagesBundle.clear();
	}
}