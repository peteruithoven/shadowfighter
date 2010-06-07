#include "SoundController.h"

SoundController::SoundController()
{
	setup();
}

void SoundController::setup()
{
	hit = &hit_Body;
	hit->setVolume(1.0f);
	hit->setMultiPlay(true);

	//bg_Demo.loadSound("sounds/bg/demo.wav");
	bg_Start.loadSound("sounds/bg/game_start.wav");
	bg_Battle.loadSound("sounds/bg/game_battle.wav");
	bg_StartBattle.loadSound("sounds/bg/game_startbattle.wav");
	bg_Intens.loadSound("sounds/bg/game_intens.wav");
	bg_StartIntens.loadSound("sounds/bg/game_startintens.wav");

	hit_Head.loadSound("sounds/hit/hit_Head.wav");
	hit_Body.loadSound("sounds/hit/hit_Body.wav");
	hit_Leg.loadSound("sounds/hit/hit_Leg.wav");
	hit_Power.loadSound("sounds/hit/hit_Power.wav");

	toFarWarning.loadSound("sounds/warning/to_far.wav");
	toFarWarning.setVolume(0.5f);
	toFarWarning.setLoop(true);
	toFarWarning.setMultiPlay(false);

	block.loadSound("sounds/block/block.wav");

	powerUp.loadSound("sounds/power_up/power_up1.wav");

	ready.loadSound("sounds/ready/ready.wav");

	//start.loadSound("sounds/start/start.wav");

	detectPlayer.loadSound("sounds/detect/detect_player.wav");

	timeUp.loadSound("sounds/time_up/time_up.wav");

	//trans.loadSound("sounds/bg/trans.wav");

	bgSound = &bg_Demo;
}

//bg sound
void SoundController::PlayBGSound(int type, int p1hp, int p2hp, int starthp, int state)
{
	//int type must be 0
	if(type != 0) return;
	switch(state)
	{
		case STATE_DEMO:
			Reset();
			break;
		case STATE_WAITING:
			Reset();
			break;
		case STATE_COUNT_DOWN:
			Reset();
			break;
		case STATE_GAME:
			PlayGameBGSound(p1hp, p2hp, starthp);
			break;
		case STATE_GAME_FINISHED:
			Reset();
			break;
	}
}

//hit sound
void SoundController::PlayHitSound(int type, int bodypart, bool power, int player)
{
	//int type must be 1
	if(type != 1) return;
	if(!power)
	{
		randomHit = ofRandom(1,3);
		switch (bodypart){
			case 1:
				hit = &hit_Head;
				break;
			case 2:
				hit = &hit_Body;
				break;
			case 3:
				hit = &hit_Leg;
				break;
			//case 4:
				//hit = &hit4;
				//break;
		};
	}
	else
	{
		hit = &hit_Power;
	}
	switch (player){
		case 1:
			hit->setPan(-1.0f);
			hit->play();
			break;
		case 2:
			hit->setPan(1.0f);
			hit->play();
			break;
	};
}

//block sound
void SoundController::PlayBlockSound(int type, int player)
{
	//int type must be 3
	if(type != 3) return;
	switch (player){
		case 1:
			block.setPan(-1.0f);
			block.play();
			break;
		case 2:
			block.setPan(1.0f);
			block.play();
			break;
	}
}

//play sound
void SoundController::PlaySound(int type, int soundstate)
{
	if(type == SOUND_WARNING)
	{
		generalSound = &toFarWarning;
		generalSound->setLoop(true);
	}
	else generalSound->setLoop(false);

	if(soundstate == 1)
	{
		if(!generalSound->getIsPlaying())
			generalSound->play();
	}
	if(soundstate == 0)
	{
		generalSound->stop();
	}
}

//general sound
void SoundController::PlaySound(int type)
{
	//int type must be bigger than 3
	switch (type)
	{
		case SOUND_BG:
			return;
			break;
		case SOUND_HIT:
			return;
			break;
		case SOUND_WARNING:
			return;
			break;
		case SOUND_BLOCK:
			return;
			break;

		case SOUND_POWERUP:
			generalSound = &powerUp;
			break;
		case SOUND_READY:
			generalSound = &ready;
			break;
		case SOUND_DETECTPLAYER:
			generalSound = &detectPlayer;
			break;
		case SOUND_TIMEUP:
			generalSound = &timeUp;
			break;
	};
	PlaySound(type,1);
}

//--------------------------------------------------

void SoundController::Reset()
{
	bgSound->stop();
}

void SoundController::PlayGameBGSound(int player1HP, int player2HP, int startingHP)
{
	//check who has the lowest hp
	if (player1HP > player2HP)
	{
		weakestPlayer = 2;
		lowestHP = player2HP;
	}
	else if(player2HP > player1HP)
	{
		weakestPlayer = 1;
		lowestHP = player1HP;
	}
	else
	{
		weakestPlayer = 0;
		lowestHP = player1HP;
	}

	//check how low the lowest hp is
	if(lowestHP < (0.3 * startingHP) && bgSound != &bg_Intens)
	{
		if(bgSound != &bg_StartIntens)
		{
			nextBGSound = 5;
			bgSound->stop();
		}
		else nextBGSound = 3;
	}
	else if (lowestHP > (0.3 * startingHP) && bgSound != &bg_Battle)
	{
		if(bgSound != &bg_StartBattle)nextBGSound = 4;
		else nextBGSound = 2;
	}

    //make sure the bg sound is playing
	if(!bgSound->getIsPlaying())
    {
        switch(nextBGSound)
		{
			case 1:
				bgSound = &bg_Start;
				break;
			case 2:
				bgSound = &bg_Battle;
				break;
			case 3:
				bgSound = &bg_Intens;
				break;
			case 4:
				bgSound = &bg_StartBattle;
				break;
			case 5:
				bgSound = &bg_StartIntens;
				break;
		}
        bgSound->setVolume(0.9f);
		bgSound->setLoop(true);
        bgSound->setMultiPlay(false);
        bgSound->play();
    }

	//difine which side is weaker
	switch(weakestPlayer)
	{
		case 1:
			bgSound->setPan(-0.4);
			break;
		case 2:
			bgSound->setPan(0.4);
			break;
		case 0:
			bgSound->setPan(0.0);
			break;
	};
}
