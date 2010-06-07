#ifndef _SoundController
#define _SoundController

#include "ofMain.h"
#include "Constants.h"

class SoundController
{
	public:
		SoundController();

		void PlayBGSound(int type, int p1hp, int p2hp, int starthp, int state);
		void PlayHitSound(int type, int bodypart, bool power, int player);
		void PlaySound(int type, int soundstate);
		void PlayBlockSound(int type, int player);
		void PlaySound(int type);

		void PlayGameBGSound(int player1HP, int player2HP, int startingHP);

		void Reset();

		ofSoundPlayer bg_Demo;
		ofSoundPlayer bg_Start;
		ofSoundPlayer bg_Battle;
		ofSoundPlayer bg_StartBattle;
		ofSoundPlayer bg_Intens;
		ofSoundPlayer bg_StartIntens;
		
		ofSoundPlayer hit_Head;
		ofSoundPlayer hit_Body;
		ofSoundPlayer hit_Leg;
		ofSoundPlayer hit_Power;
		
		ofSoundPlayer toFarWarning;
		
		ofSoundPlayer block;

		ofSoundPlayer powerUp;
		
		ofSoundPlayer ready;
		
		ofSoundPlayer detectPlayer;
		
		ofSoundPlayer timeUp;
		
		ofSoundPlayer *hit;
		ofSoundPlayer *bgSound;
		ofSoundPlayer *generalSound;
		
		float randomHit;
		
		int nextBGSound;
		int weakestPlayer;
		float lowestHP;

		void setup();
};

#endif
