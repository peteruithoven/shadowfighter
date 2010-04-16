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

Projection::Projection()
{
	//sender.setup( HOST, PORT );
	//ofAddListener(ofEvents.update, this, &Projection::update);
}
void Projection::addHit(int x,int y)
{
	//cout << "Projection::addHit: "<<x<<"x"<<y<<"\n";
	ofxOscMessage * message = new ofxOscMessage();
	//message->setAddress(ofToString(HIT_ADDRESS)+ofToString(counter));
	message->setAddress(ofToString(HIT_ADDRESS));
	message->addIntArg(x);
	message->addIntArg(y);
	messagesBundle.addMessage(*message);
}
void Projection::update(ofEventArgs & args)
{	if(messagesBundle.getMessageCount() > 0)
	{
		for(int i = 0;i<messagesBundle.getMessageCount();i++)
		{
			ofxOscMessage message = messagesBundle.getMessageAt(i);
			
			message.getArgAsInt32(0);
			message.getArgAsInt32(1);
		}
		
		sender.sendBundle(messagesBundle);
		messagesBundle.clear();
	}
}
