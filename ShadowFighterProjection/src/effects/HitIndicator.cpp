/*
 *  HitIndicatorHitIndicator.cpp
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 5/30/09.
 */

#include "HitIndicator.h"

HitIndicator::HitIndicator(int hitType)
{
	cout << "HitIndicator::HitIndicator\n";
	cout << "  hitType: "<<hitType<<"\n";
	alphaVelocity = 10; //20
	scaleVelocity = 0.05; //20
	textScaleVelocity = 0.005; //20
	
	float random;
	float randomStep;
	if(hitType == HIT_POWER)
	{
		random = ofRandom(0, 1);
		randomStep = 1.0/2*1.0;
		if(random < randomStep)
			hitImg.loadImage("images/hit/POWERHIT_1.png");	
		else if(random < randomStep*2)
			hitImg.loadImage("images/hit/POWERHIT_2.png");
	}
	else 
	{
		hitImg.loadImage("images/hit/hit.png");	   		
	}
	hitOrgW = hitImg.getWidth();
	hitOrgH = hitImg.getHeight();
	
	textX = 0;
	textY = 0;
	minRotation = 0;
	maxRotation = 360;
	random = ofRandom(0, 1);
	cout << "random: " << random << "\n";
	
	switch(hitType)
	{
		case HIT_HEAD:
			cout << "  HIT_HEAD\n";
			
			randomStep = 1.0/3*1.0;
			cout << "randomStep: " << randomStep << "\n";
			if(random < randomStep)
				textImg.loadImage("images/hit/HIT_head_1.png");	
			else if(random < randomStep*2)
				textImg.loadImage("images/hit/HIT_head_2.png");	
			else if(random < randomStep*3)
				textImg.loadImage("images/hit/HIT_head_3.png");	
			textX = ofRandom(-60,60);
			textY = ofRandom(-40,-85);
			break;
		case HIT_BODY:
			cout << "  HIT_BODY\n";
			
			randomStep = 1.0/3*0.5;
			cout << "randomStep: " << randomStep << "\n";
			if(random < randomStep)
				textImg.loadImage("images/hit/HIT_body_1.png");	
			else if(random < randomStep*2)
				textImg.loadImage("images/hit/HIT_body_2.png");	
			else if(random < randomStep*3)
				textImg.loadImage("images/hit/HIT_body_3.png");	
			   		
			textX = ofRandom(-60,60);
			textY = ofRandom(-30,-75);
			break;
		case HIT_LEGS:
			cout << "  HIT_LEGS\n";
			
			randomStep = 1.0/4*0.2;
			cout << "randomStep: " << randomStep << "\n";
			if(random < randomStep)
				textImg.loadImage("images/hit/HIT_legs_1.png");	
			else if(random < randomStep*2)
				textImg.loadImage("images/hit/HIT_legs_2.png");	
			else if(random < randomStep*3)
				textImg.loadImage("images/hit/HIT_legs_3.png");	
			else if(random < randomStep*4)
				textImg.loadImage("images/hit/HIT_legs_4.png");	
			
			textX = ofRandom(-60,60);
			textY = ofRandom(-25,-70);
			break;
		case HIT_POWER:
			cout << "  HIT_POWER\n";
			
			alphaVelocity = 5; //20
			scaleVelocity = 0.02; //20
			minRotation = -30;
			maxRotation = 30;
			
			break;
	}
	
	textX -= textImg.getWidth()/2;
	textY -= textImg.getHeight()/2;
		
	start();
}
void HitIndicator::start()
{
	width = 0;
	bangImgY = 0-bangImg.getHeight();
	hitScale = 0.4;
	textScale = 0.7;
	rotation = ofRandom(minRotation,maxRotation);
	alpha = 200;
	running = true;
}
void HitIndicator::draw()
{ 
	//cout << "HitIndicator::draw\n";
	
	ofFill();
	ofEnableAlphaBlending();
	ofSetColor(255,255,255,alpha);
	//ofSetColor(255,255,255,255);
	//ofCircle(x, y, width*scale);
	//bangImg.draw(x-bangImg.getWidth()/2, y+bangImgY);
	
	ofPushMatrix();
	ofTranslate(x, y, 0);
	ofScale(hitScale, hitScale, 1);
	ofRotate(rotation);
	hitImg.draw(0-hitImg.getWidth()/2,0-hitImg.getHeight()/2);
	ofPopMatrix();
	
	
	//textScaleVelocity
	ofPushMatrix();
	ofTranslate(x, y, 0);
	ofScale(textScale, textScale, 1);
	textImg.draw(textX,textY);
	ofPopMatrix();
	
	ofSetColor(255,255,255,255);
	ofDisableAlphaBlending();
	
	//ofCircle(x, y, 10);
	
	width += 8;
	alpha -= alphaVelocity;
	bangImgY -= 2;
	hitScale += scaleVelocity; //0.05;
	textScale += textScaleVelocity;
	if(alpha < 0)
		stop();
}