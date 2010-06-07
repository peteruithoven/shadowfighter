/*
 *  SettingsView.cpp
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 4/12/10.
 *  Copyright 2010 HKU. All rights reserved.
 *
 */

#include "ProjectionView.h"

ProjectionView::ProjectionView()
{
	
}

void ProjectionView::setModel(Model * model)
{
	cout << "ProjectionView::setModel\n";
	this->model = model;
	
//	if(model->pixelsSource == Model::CAMERA)
//	{
		ofAddListener(model->HIT,this,&ProjectionView::onHit);
		ofAddListener(model->BLOCK,this,&ProjectionView::onBlock);
		ofAddListener(model->RESET,this,&ProjectionView::onReset);
		ofAddListener(model->STATE_CHANGE,this,&ProjectionView::onStateChange);
		ofAddListener(model->PLAYERS_CHANGED,this,&ProjectionView::onPlayersChanged);
		ofAddListener(model->COUNT_DOWN,this,&ProjectionView::onCountDown);
		ofAddListener(model->BLOCKING,this,&ProjectionView::onBlocking);
//	}
	projection.model = model;
	
	projection.updateState(model->state);
}
void ProjectionView::onHit(HitVO & hitVO)
{
//	cout << "ProjectionView::onHit\n";
//	ofRectangle bounds = hitVO.bounds;
//	int x = bounds.x+bounds.width/2;
//	int y = bounds.y+bounds.height/2;
	projection.addHit(hitVO);
	
	updateHealth();
}
void ProjectionView::onBlock(BlockVO & blockVO)
{
	//cout << "ProjectionView::onBlock\n";
//	ofRectangle bounds = blockVO.bounds;
//	int x = bounds.x+bounds.width/2;
//	int y = bounds.y+bounds.height/2;
	projection.addBlock(blockVO);
	
	updateHealth();
}
void ProjectionView::onBlocking(BlockVO & blockVO)
{
	//cout << "ProjectionView::onBlocking\n";
	projection.addBlocking(blockVO);
}
void ProjectionView::onReset(int & arg)
{
	updateHealth();
}
void ProjectionView::onStateChange(int & arg)
{
	cout << "ProjectionView::onStateChange\n";
	if(model->state == STATE_GAME_FINISHED)
		projection.updateWinner(model->winner);
	projection.updateState(model->state);
}
void ProjectionView::onPlayersChanged(int & arg)
{
	cout << "ProjectionView::onPlayersChanged\n";
	projection.updatePlayers(model->detectedPlayer1, model->detectedPlayer2);
}
void ProjectionView::onCountDown(int & arg)
{
	cout << "ProjectionView::onCountDown\n";
	projection.updateCountDown(model->countDown);
}

void ProjectionView::updateHealth()
{
	float player1Health = float(model->player1Health)/model->startHealth;
	float player2Health = float(model->player2Health)/model->startHealth;
	projection.updateHealth(player1Health, player2Health);
}