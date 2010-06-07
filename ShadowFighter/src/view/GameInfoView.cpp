/*
 *  GameInfoView.cpp
 *  ShadowFighter
 *
 *  Created by Peter Uithoven on 4/24/10.
 */

#import "GameInfoView.h"


GameInfoView::GameInfoView()
{
	
}

void GameInfoView::setModel(Model * model)
{
	this->model = model;
	ofAddListener(model->HIT,this,&GameInfoView::onHit);
	ofAddListener(model->RESET,this,&GameInfoView::onReset);
	
	int videoW = model->videoW;
	
	
	//healthBarPlayer1.x = (videoW/2-healthBarPlayer1.width)/2;
	//healthBarPlayer2.x = (videoW/2-healthBarPlayer2.width)/2+videoW/2;
	int marginX = (videoW-healthBarPlayer1.width-healthBarPlayer2.width)/3;
	healthBarPlayer1.x = marginX;
	healthBarPlayer2.x = videoW-healthBarPlayer2.width-marginX;
	healthBarPlayer1.y = healthBarPlayer2.y = 10;
	
	update();
}
void GameInfoView::onHit(HitVO & hitVO)
{
	cout << "GameInfoView::onHit\n";
	update();
}
void GameInfoView::onReset(int & arg)
{
	cout << "GameInfoView::onReset\n";
	update();
}
void GameInfoView::update()
{
	healthBarPlayer1.percentage = float(model->player1Health)/model->startHealth;
	healthBarPlayer2.percentage = float(model->player2Health)/model->startHealth;
}
