/*
 *  GameInfoView.h
 *  ShadowFighter
 *
 *  Created by Peter Uithoven on 4/24/10.
 */

#ifndef _GameInfoView
#define _GameInfoView

#include "BaseView.h"
#include "ofMain.h"
#include "HealthBar.h"

class GameInfoView : public BaseView 
{
	public:
	
		HealthBar healthBarPlayer1;
		HealthBar healthBarPlayer2;
	
		GameInfoView();
		void setModel(Model * model);
	
	private:
		void onHit(HitVO & hitVO);
		void onReset(int & arg);
		void update();
	
};

#endif


