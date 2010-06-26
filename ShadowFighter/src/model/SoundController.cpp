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

	bg_Demo.loadSound("sounds/bg/demo.wav");
	bg_Battle.loadSound("sounds/bg/game_battle.wav");
	bg_StartBattle.loadSound("sounds/bg/game_startbattle.wav");
	bg_Intens.loadSound("sounds/bg/game_intens.wav");
	bg_StartIntens.loadSound("sounds/bg/game_startintens.wav");
	bg_Win.loadSound("sounds/bg/win.wav");

	hit_Head.loadSound("sounds/hit/hit_Head.wav");
	hit_Body.loadSound("sounds/hit/hit_Body.wav");
	hit_Leg.loadSound("sounds/hit/hit_Leg.wav");
	hit_Power.loadSound("sounds/hit/hit_Power.wav");

	toFarWarning.loadSound("sounds/warning/to_far.wav");
	toFarWarning.setVolume(2.0f);
	//toFarWarning.setLoop(true);
	toFarWarning.setMultiPlay(false);

	block.loadSound("sounds/block/block.wav");
	block.setVolume(10.0);
	
	block_pb.loadSound("sounds/block/block_pb.wav");
	block_pb.setVolume(10.0);

	powerUp.loadSound("sounds/power_up/power_up1.wav");

	ready.loadSound("sounds/ready/ready.wav");

	detectPlayer.loadSound("sounds/detect/detect_player.wav");

	timeUp.loadSound("sounds/time_up/time_up.wav");

	bgSound = &bg_Demo;

	fadeOutTimerSet = 3.0f;
	fadeOutTimer = fadeOutTimerSet;
}



//bg sound
void SoundController::PlayBGSound(int type, int p1hp, int p2hp, int starthp, int state)
{
	if (fadeOut)
	{
		bgSound->setVolume((0.7f / fadeOutTimerSet) * fadeOutTimer);
		fadeOutTimer -= 1.0f/ ofGetFrameRate();
	}
	else fadeOutTimer = fadeOutTimerSet;

	if (fadeOutTimer <= 0.01f)
	{
		//if(bgSound == fadeSound)Reset();
		bgSound->stop();
		bgSound->setVolume(0.7);
		fadeOut = false;
	}
	//int type must be sound_bg
	if(type != SOUND_BG) return;
	
	if(previousState != state)
	{
		if(state != STATE_COUNT_DOWN)
		{
			bgSound->stop();
			bgSound->setVolume(1.0);
			fadeOut = false;
		}
		else fadeOut = true;
	}
	switch(state)
	{
		case STATE_DEMO:
			//if(bgSound != &bg_Demo)bgSound->setVolume(1.0);
			bgSound = &bg_Demo;
			PlaySound(SOUND_BG, 1);
			break;
		case STATE_WAITING:
			Reset();
			break;
		case STATE_COUNT_DOWN:
			//fadeOut = true;

			//if (!bgSound->getIsPlaying())
			//bgSound = &bg_Demo;
			//PlaySound(SOUND_BG, 0);
			break;
		case STATE_GAME:
			//bgSound->setVolume(1.0);
			//if(bgSound != &bg_StartBattle || bgSound != &bg_Battle || bgSound != &bg_Intens || bgSound != &bg_StartIntens)bgSound->stop();
			PlayGameBGSound(p1hp, p2hp, starthp);
			break;
		case STATE_GAME_FINISHED:
			//if(bgSound !=  &bg_Win)bgSound->stop();
			PlaySound(SOUND_BG, 0);
			break;
	}
	previousState = state;
	//cout << previousState << "\n";
}

//hit sound
void SoundController::PlayHitSound(int type, int bodypart, bool power, int player)
{
	//int type must be sound_hit
	if(type != SOUND_HIT) return;
	if(!power)
	{
		randomHit = ofRandom(1,4);
		switch ((int)randomHit){
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
void SoundController::PlayBlockSound(int type, int player, bool powerHit)
{
	//int type must be sound_block
	if(type != SOUND_BLOCK) return;
	
	ofSoundPlayer *blockSound = (powerHit)? &block_pb : &block;
	
	switch (player){
		case 1:
			blockSound->setPan(-1.0f);
			blockSound->play();
			break;
		case 2:
			blockSound->setPan(1.0f);
			blockSound->play();
			break;
	}
}

//play sound
void SoundController::PlaySound(int type, int soundstate)
{
	//generalSound->setVolume(1.0);
	if(type == SOUND_WARNING)
	{
		generalSound = &toFarWarning;
		//generalSound->setLoop(true);
		generalSound->setVolume(2.0);
	}
	else if (type == SOUND_BG)
	{
		switch (soundstate)
		{
			case 0:
			if(!bgSound->getIsPlaying() && bgSound != &bg_Win)
			{
				bgSound = &bg_Win;
				bgSound->play();
				bgSound->setLoop(false);
			}
			break;
			case 1:
			if(!bgSound->getIsPlaying())
			{
				bgSound->play();
				bgSound->setLoop(true);
				//bgSound->setVolume(0.7f);
			}
			break;
		}
		return;
	}
	else generalSound->setLoop(false);

	switch (soundstate)
	{
		case 0:
		generalSound->stop();
		break;
		case 1:
		if(!generalSound->getIsPlaying())
		generalSound->play();
		break;
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
			bgSound->setLoop(false);
		}
		else nextBGSound = 3;
	}
	else if (lowestHP > (0.3 * startingHP) && bgSound != &bg_Battle)
	{
		if(bgSound != &bg_StartBattle)
		{
			nextBGSound = 4;
			bgSound->setLoop(false);
		}
		else nextBGSound = 2;
	}

    //make sure the bg sound is playing
	if(!bgSound->getIsPlaying())
    {
        switch(nextBGSound)
		{
			//case 1:
				//bgSound = &bg_Start;
				//break;
			case 2:
				bgSound = &bg_Battle;
				bgSound->setLoop(true);
				break;
			case 3:
				bgSound = &bg_Intens;
				bgSound->setLoop(true);
				break;
			case 4:
				bgSound = &bg_StartBattle;
				break;
			case 5:
				bgSound = &bg_StartIntens;
				break;
		}
        bgSound->setVolume(0.9f);
		//bgSound->setLoop(true);
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
