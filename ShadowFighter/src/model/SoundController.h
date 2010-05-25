#ifndef _SoundController
#define _SoundController

#include "ofMain.h"

class SoundController
{
	public:
		SoundController();
		void PlaySound(int player);
		void PlayBGSound(int player1HP, int player2HP, int startingHP);
		void Reset();

		ofSoundPlayer hit1;
		ofSoundPlayer hit2;
		ofSoundPlayer hit3;
		ofSoundPlayer hit4;
		ofSoundPlayer block;
		ofSoundPlayer toFarWarning;

		ofSoundPlayer *hit;
		float randomHit;
		
		ofSoundPlayer bg1;
		ofSoundPlayer bg2;
		ofSoundPlayer bg3;
		
		ofSoundPlayer *bgSound;
		int nextBGSound;
		int weakestPlayer;
		float lowestHP;

		void setup();
		void startToFarWarning();
		void stopToFarWarning();
		void PlayBlockSound(int player);
};

#endif
