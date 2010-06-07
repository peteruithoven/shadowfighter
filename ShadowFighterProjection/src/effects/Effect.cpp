/*
 *  Effect.cpp
 *  ShadowFighterProjection
 *
 *  Created by Peter Uithoven on 5/25/10.
 */

#import "Effect.h"

Effect::Effect()
{
	running = false;
}
void Effect::start()
{
	running = true;
}
void Effect::stop()
{
	running = false;
}