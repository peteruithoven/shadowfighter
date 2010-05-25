#include "SoundController.h"

SoundController::SoundController()
{
	setup();
}

//defauld bg audio class

void SoundController::setup()
{
	hit = &hit1;
	hit->setVolume(1.0f);
	hit->setMultiPlay(true);
	
	hit1.loadSound("sounds/hit/hit3-eliad.wav");
	hit2.loadSound("sounds/hit/hit3-eliad.wav");
	hit3.loadSound("sounds/hit/hit1-maurus.wav");
	hit4.loadSound("sounds/hit/hit1-maurus.wav"); //hit1-maurus
	
	block.loadSound("sounds/block/block.wav"); //hit1-maurus
	
	bg1.loadSound("sounds/bg/theme1.mp3");
	bg2.loadSound("sounds/bg/theme1.mp3");
	bg3.loadSound("sounds/bg/theme1.mp3");
	
	toFarWarning.loadSound("sounds/warning/to-far.wav");
	toFarWarning.setVolume(0.5f);
	//toFarWarning.setLoop(true);
	toFarWarning.setMultiPlay(false);
	
	bgSound = &bg1;
}

void SoundController::PlaySound(int player)
{

	randomHit = ofRandom(1,4);
	switch ((int)randomHit){
		case 1:
			hit = &hit1;
			break;
		case 2:
			hit = &hit2;
			break;
		case 3:
			hit = &hit3;
			break;
		case 4:
			hit = &hit4;
			break;
	};

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
void SoundController::PlayBlockSound(int player)
{
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
void SoundController::PlayBGSound(int player1HP, int player2HP, int startingHP)
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
//	if(lowestHP < (0.11 * startingHP) && bgSound != &bg3)
//	{
//		bgSound->setLoop(false);
//		nextBGSound = 3;
//	}
//	else if(lowestHP < (0.5 * startingHP) && lowestHP > (0.11 * startingHP) && bgSound != &bg2)
//	{
//		bgSound->setLoop(false);
//		nextBGSound = 2;
//	}
//	else if (lowestHP > (0.5 * startingHP) && bgSound != &bg1)
//	{
//		nextBGSound = 1;
//	}

    //make sure the bg sound is playing
	if(!bgSound->getIsPlaying())
    {
        switch(nextBGSound)
		{
			case 1:
				bgSound = &bg1;
				break;
			case 2:
				bgSound = &bg2;
				break;
			case 3:
				bgSound = &bg3;
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

void SoundController::Reset()
{
	bgSound->stop();
}

void SoundController::startToFarWarning()
{
	cout << "SoundController::startToFarWarning\n";
	if(!toFarWarning.getIsPlaying())
		toFarWarning.play();
}
void SoundController::stopToFarWarning()
{
	//cout << "SoundController::stopToFarWarning\n";
	if(toFarWarning.getIsPlaying())
		toFarWarning.stop();
}