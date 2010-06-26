/*
 *  VisualFeedback.h
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 5/30/09.
 *
 */

#ifndef _Projection
#define _Projection

#include "ofMain.h"
#include "ofxOsc.h"
#include "DisplayObject.h"
#include "Model.h"
#include "HitVO.h"
#include "BlockVO.h"


class Projection : public DisplayObject 
{

	public:
	
		Model * model;
		
		Projection();
		void addHit(HitVO hitVO);
		void addBlock(BlockVO blockVO);
		void addBlocking(BlockVO blockVO);
		void updateHealth(float player1Health,float player2Health);
		void updateState(int state);
		void updateWinner(int winner);
		void updatePlayers(bool detectedPlayer1, bool detectedPlayer2);
		void updateCountDown(int countDown);
		void updatePowerhits(bool powerHit,bool powerHit);
	
	private:
		ofxOscSender sender;
		ofxOscBundle messagesBundle;
		void update(ofEventArgs & args);
	
};

#endif