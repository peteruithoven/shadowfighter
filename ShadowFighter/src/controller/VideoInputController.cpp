/*
 *  VideoInputController.cpp
 *  openFrameworks
 *
 *  Created by Peter Uithoven on 4/13/10.
 *  Copyright 2010 HKU. All rights reserved.
 *
 */

#include "VideoInputController.h"
#include "ofxCvColorImage.h"

VideoInputController::VideoInputController()
{
	font.loadFont("mono.ttf", 10);
}
bool sortBlobsOnX(ofxCvBlob * blob1, ofxCvBlob * blob2)
{
	return (blob1->boundingRect.x < blob2->boundingRect.x); 
}
bool sortRectsOnX(ofRectangle * rect1, ofRectangle * rect2)
{
	return (rect1->x < rect2->x); 
}
void VideoInputController::newPixels(unsigned char * pixels)
{
	if(model->state != STATE_GAME_FINISHED)
		analyze(pixels);
}
void VideoInputController::analyze(unsigned char * pixels)
{
	//cout << "VideoInputController::analyze\n";
	model->frameCounter++;
	
	int videoW = model->videoW;
	int videoH = model->videoH;
	
	// convert color pixels to grayscale pixels
	unsigned char * grayPixels = toGrayscale(pixels);
	model->grayImg->setFromPixels(grayPixels, videoW,videoH);
	// Optionally store empty frame image
	if (model->willLearnBackground && model->pixelsSource == Model::CAMERA)
	{
		storeBackgroundImage(grayPixels);
		model->willLearnBackground = false;
	}
	
	correctEmptyImage();
	
	if(model->debugDetection)
		model->grayEmptyImg->draw(videoW, 0);
	delete [] grayPixels;
	
	// filters
	filterProjection();
	
	findHitBlobs();

	findShadowBlobs();
	
	//if(model->debugDetection)
	//	drawBlobsHistory();
	
	analyzeShadows();
	
//	if(model->state == STATE_GAME)
//	{
//		//analyzeShadows();
//		
		analyseHitBlobsSimple(pixels);
		
		storeShadowBlobs();
		
		storeHistory();
//	}
	takeScreenShot("");
}
void VideoInputController::storeBackgroundImage(unsigned char * grayPixels)
{
	//cout << "VideoInputController::storeBackgroundImage\n";
	model->grayEmptyImg->setFromPixels(grayPixels,model->videoW,model->videoH);
	
	ofImage imgSaver = *new ofImage();
	imgSaver.setFromPixels(grayPixels, model->videoW, model->videoH, OF_IMAGE_GRAYSCALE, false);
	imgSaver.saveImage("empty.png");
}
void VideoInputController::filterProjection()
{
	//cout << "VideoInputController::filterProjection\n";
	int videoW = model->videoW;
	int videoH = model->videoH;
	
	unsigned char * backgroundPixels = model->grayEmptyImg->getPixels();
	
	// check for projection
	unsigned char * pixels = model->grayImg->getPixels();
	for (int i = 0; i < videoH; i++) {
		for (int j = 0; j < videoW; j++) {
			
			int pixelIndex = (i*videoW) + j;
			if(model->pixelsSource == Model::CLIP5_DEMO)
			{
				pixels[pixelIndex] -= 30;
			}
			/*else if(model->pixelsSource == Model::CLIP6_DEMO)
			{
				//cout << "pixels[pixelIndex]: " << pixels[pixelIndex] << "\n";
				pixels[pixelIndex] -= model->clip6Correction; //49;
				//cout << "->pixels[pixelIndex]: " << pixels[pixelIndex] << "\n";
				//if(pixels[pixelIndex] < 0)
					//pixels[pixelIndex] = 0;
			}*/
			if(pixels[pixelIndex] > 180)
			{
				pixels[pixelIndex] = backgroundPixels[pixelIndex];
			}
		}
	}
	model->grayImg->setFromPixels(pixels, videoW, videoH);
	
	//cout << "  model->clip6Correction: " << model->clip6Correction << "\n";
	
	//if(model->debugDetection)
	//	model->grayImg->draw(0,0);
}
void VideoInputController::correctEmptyImage()
{
	if(model->pixelsSource != Model::CLIP6_DEMO)
		return;
	
	int videoW = model->videoW;
	int videoH = model->videoH;
	
	unsigned char * orgPixels = model->grayEmptyCopyImg->getPixels();
	//model->grayEmptyImg->setFromPixels(orgPixels, videoW, videoH);
	
	//unsigned char * pixels = model->grayEmptyCopyImg->getPixels();
	
	unsigned char* newPixels = new unsigned char[videoW*videoH];
	
	for (int i = 0; i < videoH; i++) 
	{
		for (int j = 0; j < videoW; j++) 
		{
			int pixelIndex = (i*videoW) + j;
			newPixels[pixelIndex] = orgPixels[pixelIndex] + model->clip6EmptyCorrection;
		}
	}
	model->grayEmptyImg->setFromPixels(newPixels, videoW, videoH);
	delete [] newPixels;
}
void VideoInputController::findHitBlobs()
{
	//cout << "VideoInputController::findHitBlobs\n";
	int hitBlobDisplayX = 0;
	int hitBlobDisplayY = model->videoH;
	
	model->grayHitDiffImg->absDiff(*model->grayEmptyImg, *model->grayImg);
	//model->grayHitDiffImg->draw(0, videoH);
	
	model->grayHitDiffImg->threshold(model->threshold+model->hitThreshold);
	if(model->debugDetection)
		model->grayHitDiffImg->draw(hitBlobDisplayX, hitBlobDisplayY);
	
	hitContourFinder.findContours(*model->grayHitDiffImg, model->minHitBlobArea, model->maxHitBlobArea, model->maxNumHitBlobs, true);
	//hitContourFinder.findContours(*model->grayHitDiffImg, 5, 640*480, 1000, false);
}
void VideoInputController::findShadowBlobs()
{
	// take the abs value of the difference between background and incoming and then threshold:
	model->grayDiffImg->absDiff(*model->grayEmptyImg, *model->grayImg);
	model->grayDiffImg->threshold(model->threshold);
	
	
	
	// loop through pixels of grayHitDiffImg if white, 
	//   paint same pixel index in grayDiffImg black
	//cout << "VideoInputController::filterProjection\n";
	int videoW = model->videoW;
	int videoH = model->videoH;
	
	//unsigned char * grayHitDiffPixels = model->grayHitDiffImg->getPixels();
	//unsigned char * grayDiffPixels = model->grayDiffImg->getPixels();
	/*for (int i = 0; i < videoH; i++) {
		for (int j = 0; j < videoW; j++) {
			
			int pixelIndex = (i*videoW) + j;
			
			if(grayHitDiffPixels[pixelIndex] == 255)
			{
				int tPI = ((i-1)*videoW) + j;
				int bPI = ((i+1)*videoW) + j;
				int lPI = (i*videoW) + (j-1);
				int rPI = (i*videoW) + (j+1);
				
				if(tPI >= 0) grayHitDiffPixels[tPI] = 200;
				if(bPI >= 0) grayHitDiffPixels[bPI] = 200;
				if(lPI >= 0) grayHitDiffPixels[lPI] = 200;
				if(rPI >= 0) grayHitDiffPixels[rPI] = 200;
			}
		}
	}
	for (int i = 0; i < videoH; i++) {
		for (int j = 0; j < videoW; j++) {
			
			int pixelIndex = (i*videoW) + j;
			
			if(grayHitDiffPixels[pixelIndex] == 200)
			{
				grayHitDiffPixels[pixelIndex] = 255;
			}
		}
	}*/
	/*for (int i = 0; i < videoH; i++) {
		for (int j = 0; j < videoW; j++) {
			
			int pixelIndex = (i*videoW) + j;
			
			if(grayHitDiffPixels[pixelIndex] == 255)
				grayDiffPixels[pixelIndex] = 0;
		}
	}
	model->grayDiffImg->setFromPixels(grayDiffPixels, videoW, videoH);*/
	
	/*ofRectangle * hitDetectionZone = &model->hitDetectionZone;
	int increase = 10;
	for(int i = 0; i<hitContourFinder.blobs.size();i++)
	{
		ofxCvBlob hitBlob = hitContourFinder.blobs[i];
		ofRectangle hitBlobRect = hitBlob.boundingRect;
		
		if((hitBlobRect.x >= hitDetectionZone->x &&
			 hitBlobRect.y >= hitDetectionZone->y &&
			 hitBlobRect.x+hitBlobRect.width <= hitDetectionZone->x+hitDetectionZone->width &&
			 hitBlobRect.y+hitBlobRect.height <= hitDetectionZone->y+hitDetectionZone->height))
		{
			hitBlobRect.x -= increase/2;
			hitBlobRect.y -= increase/2;
			hitBlobRect.width += increase;
			hitBlobRect.height += increase;
			colorInImage(model->grayDiffImg, 0, hitBlobRect);
		}
	}*/
	
	
	if(model->pixelsSource == Model::CLIP6_DEMO)
	{
		ofRectangle colorRect;
		colorRect.width = model->videoW;
		colorRect.height = 50;
		colorRect.x = 0;
		colorRect.y = model->videoH-colorRect.height;
		colorInImage(model->grayDiffImg, 0x000000, colorRect);
	}
	
	if(model->debugDetection)
		model->grayDiffImg->draw(model->videoW, model->videoH);
	// find shadow/normal blobs
	contourFinder.findContours(*model->grayDiffImg, model->minBlobArea, model->maxBlobArea, model->maxNumBlobs, true);
	
	
	float blobDisplayX = model->videoW;
	float blobDisplayY = model->videoH;
	ofRectangle *detectionZone = &model->detectionZone;
	if(model->debugDetection)
	{
		ofNoFill();
		ofSetColor(0x00cc00);
		ofRect(detectionZone->x, detectionZone->y, detectionZone->width, detectionZone->height);
		ofRect(blobDisplayX+detectionZone->x, blobDisplayY+detectionZone->y, detectionZone->width, detectionZone->height);
	}
	
	//model->currentBlobs->clear();
	vector<Blob*> * currentBlobs = new vector<Blob*>; 
    for (int i = 0; i < contourFinder.blobs.size(); i++)
	{
		ofxCvBlob blob = contourFinder.blobs[i];
		ofRectangle blobRect = blob.boundingRect;
		/*if(!(blobRect.x >= detectionZone->x &&
		 blobRect.y >= detectionZone->y &&
		 blobRect.x+blobRect.width <= detectionZone->x+detectionZone->width &&
		 blobRect.y+blobRect.height <= detectionZone->y+detectionZone->height))
		 continue;*/
		
		/*int blobRectCX = blobRect.x+blobRect.width/2;
		 int blobRectCY = blobRect.y+blobRect.height/2;
		 if(!(blobRectCX >= detectionZone->x &&
		 blobRectCY >= detectionZone->y &&
		 blobRectCX <= detectionZone->x+detectionZone->width &&
		 blobRectCY <= detectionZone->y+detectionZone->height))
		 continue;*/
		
		//blob.draw(0,0);
		if(model->debugDetection)
			blob.draw(blobDisplayX,blobDisplayY);
		
		// store copy of blobs
		Blob * blobCopy = new Blob();
		blobCopy->boundingRect.x = blobRect.x;
		blobCopy->boundingRect.y = blobRect.y;
		blobCopy->boundingRect.width = blobRect.width;
		blobCopy->boundingRect.height = blobRect.height;
		//cout << "  new blob copy: " << blobCopy << "\n";
		//model->currentBlobs->push_back(blobCopy);
		currentBlobs->push_back(blobCopy);
	}
	model->blobsHistory->push_back(currentBlobs);
	if(model->blobsHistory->size() > model->maxBlobsHistoryLength)
	{
		vector<Blob*> * blobs = model->blobsHistory->at(0);
		delete blobs;
		model->blobsHistory->erase(model->blobsHistory->begin());
	}
}
/* 
 * Analyze the full shadow's to detect main body and block parts
 */
void VideoInputController::analyzeShadows()
{
	//cout << "VideoInputController::analyzeShadows\n";
	int w = model->videoW;
	int h = model->videoH;
	int dx = model->detectionZone.x;
	int dy = model->detectionZone.y;
	int dw = model->detectionZone.width;
	int dh = model->detectionZone.height;
	int minBlockBodyDis = 5;
	int minBodyH = dh*0.15;
	int minMainBodyH = dh*0.5; //dh*0.4; //dh*0.6; //h/2;
	int minBlockH = dh*0.15; //h/8;
	int minBlockWidth = 3;
	int maxBlockWidth = 100; //75;
	int minMainBodyWidth = 20;
	int minBodyWidth = 50;
	int extraBlockHeightT = 10;
	int extraBlockHeightB = 20;

	
	//cout << "dw: " << dw << "\n";
	//cout << "dh: " << dh << "\n";
	//cout << "minBodyH: " << minBodyH << "\n";
	//cout << "minBlockH: " << minBlockH << "\n";
	
	for(int i=0;i<model->bodies.size();i++)
	{
		delete model->bodies.at(i);
	}
	model->bodies.clear();
	for(int i=0;i<model->mainBodies.size();i++)
	{
		delete model->mainBodies.at(i);
	}
	model->mainBodies.clear();
	for(int i=0;i<model->blocks.size();i++)
	{
		delete model->blocks.at(i);
	}
	model->blocks.clear();
	
	unsigned char *blobsPixels = model->grayDiffImg->getPixels();
	unsigned char *pixels;
	ofImage debugImage;
	if(model->debugDetection)
	{
		debugImage.allocate(w, h, OF_IMAGE_COLOR_ALPHA);
		pixels = debugImage.getPixels();
	}
	
	//vector<ofRectangle*> model->bodies;
	//vector<ofRectangle*> blocks;
	ofRectangle * body = NULL;
	ofRectangle * mainBody = NULL;
	ofRectangle * block = NULL;
	
	for (int x = dx; x < dx+dw; x++) 
	{
		int pixelsY = -1;
		int totalPixelsY = -1;
		int numPixelsY = 0;
		int numTotalPixelsY = 0;
		for (int y = dy; y < dy+dh; y++) 
		{
			int blobsPixelIndex = (y*w) + x;
			
			if(blobsPixels[blobsPixelIndex] == 255)
			{
				if(pixelsY == -1)
					pixelsY = y;
				if(totalPixelsY == -1)
					totalPixelsY = y;
				numPixelsY++;
				numTotalPixelsY++;
				
				// if bottom pixel?
				if(y == (dy+dh)-1)
					addBlock(x,pixelsY,1,numPixelsY);
			}
			else 
			{
				if(pixelsY != -1)
				{
					if(numPixelsY < minMainBodyH && numPixelsY > minBlockH) // && (blockDifY < 110 || block.height == 0))
					{
						// add block	
						addBlock(x,pixelsY,1,numPixelsY);
					}
				}
				pixelsY = -1;
				numPixelsY = 0;
			}

			/*// vertical blob ended
			else if(pixelsY != -1) // ||  && x < dx+dw-1)
			{
				
			}*/
		}
		
		if(model->debugDetection)
		{
			for (int y = dy; y < dy+dh; y++) 
			{
				int pixelIndex = ( (y*w) + x ) *4;
				
				int r = 0;
				int g = 0;
				int b = 0;
				int a = 0;
				if(y > dh-numTotalPixelsY+dy)  //dh-numPixelsY+dy*2)
				{
					g = 255;
					a = 125;
				}
				if(numTotalPixelsY > minMainBodyH)
				{
					b = g;
					g = 0;
				}
				else if(numTotalPixelsY > minBlockH) // && (blockDifY < 110 || block.height == 0))
				{
					r = g;
					g = 0;
				}
				
				pixels[pixelIndex+0] = r;
				pixels[pixelIndex+1] = g;
				pixels[pixelIndex+2] = b;
				pixels[pixelIndex+3] = a;
			}
			
		}
		
		// when there are enough vertical pixels for it to be the main body...
		if(numTotalPixelsY > minMainBodyH && x < dx+dw-1)
		{
			// add mainBody
			if(mainBody == NULL)
			{
				mainBody = new ofRectangle();
				mainBody->x = x;
				mainBody->y = totalPixelsY;
				mainBody->height = numTotalPixelsY;
			}
			// update body
			else 
			{
				if(mainBody->y > totalPixelsY)
					mainBody->y = totalPixelsY;
				mainBody->width = x-mainBody->x;
				if(numTotalPixelsY > mainBody->height)
					mainBody->height = numTotalPixelsY;
			}
		}
		// when there aren't
		else {
			if(mainBody != NULL)
			{
				// store main body
				model->mainBodies.push_back(mainBody);
				mainBody = NULL;
			}
		}
		
		// when there are enough vertical pixels for it to be the body...
		if(numTotalPixelsY > minBodyH && x < dx+dw-1)
		{
			// add body
			if(body == NULL)
			{
				body = new ofRectangle();
				body->x = x;
				body->y = totalPixelsY;
				body->height = numTotalPixelsY; //to lowest pixel
			}
			// update body
			else 
			{
				if(body->y > totalPixelsY)
					body->y = totalPixelsY;
				body->width = x-body->x;
				if(numTotalPixelsY > body->height)
					body->height = numTotalPixelsY;
			}
		}
		// when there aren't
		else {
			if(body != NULL)
			{
				// store main body
				model->bodies.push_back(body);
				body = NULL;
			}
		}
	}
	
	if(model->debugDetection)
	{
		debugImage.setFromPixels(pixels, w, h, OF_IMAGE_COLOR_ALPHA, true);	
		
		ofSetColor(255, 255, 255);
		ofEnableAlphaBlending();
		debugImage.draw(w, h);
		ofDisableAlphaBlending();
	}
	
	// merge touching blocks
	int maxBlockMergeDis = 2; //5;
	if(model->blocks.size() > 0)
		std::sort(model->blocks.begin(), model->blocks.end(), &sortRectsOnX);
	
	ofRectangle *prevBlock = NULL;
	for(int i=0;i<model->blocks.size();i++)
	{
		ofRectangle *block = model->blocks.at(i);
		
		int dis = 0;
		if(prevBlock != NULL)
		{
			dis = block->x-(prevBlock->x+prevBlock->width);
			
			//&& prevBody.x 
			//cout << "    dis: " << dis << "\n";
			if(dis < maxBlockMergeDis)
			{
				//cout << "      merge!\n";
				block = mergeRectangles(block,prevBlock);
				model->blocks[i] = block;
				model->blocks.erase(model->blocks.begin()+(i-1));
				i--;
			}
		}
		prevBlock = block;
	}
	
	
	
	for(int i=0;i<model->blocks.size();i++)
	{
		ofRectangle * block = model->blocks.at(i);
		
		// block width check
		if(block->width > maxBlockWidth || block->width < minBlockWidth)
		{
			if(block->width > maxBlockWidth)
				model->mainBodies.push_back(block);
			model->blocks.erase(model->blocks.begin()+i);
			i--;
			continue;
		}
		// block isn't touching body check
		//cout << "  block isn't touching body check\n";
		bool blockIsTouchingBody = false;
		for(int j=0;j<model->mainBodies.size();j++)
		{
			ofRectangle *mainBody = model->mainBodies.at(j);
			
			int s1 = block->width/2;
			int s2 = mainBody->width/2;
			int disX = (block->x+s1)-(mainBody->x+s2);
			if(disX < 0) disX *= -1;
			//cout << "    disX: " << disX << "\n";
			
			int maxDis = s1+s2+minBlockBodyDis;
			//cout << "    maxDis: " << maxDis << "\n";
			if(disX < maxDis)
			{
				blockIsTouchingBody = true;
				continue;
			}
		}
		//cout << "  blockIsTouchingBody: " << blockIsTouchingBody << "\n";
		if(blockIsTouchingBody)
		{
			//cout << "    erase\n";
			model->blocks.erase(model->blocks.begin()+i);
			delete block;
			//model->bodies.push_back(block);
			//model->mainBodies.push_back(block);
//			blocks.erase(blocks.begin()+i);
			i--;
			continue;
		}
		
		block->y -= extraBlockHeightT;
		block->height += extraBlockHeightT+extraBlockHeightB;
		
		if(model->debugDetection)
		{
			ofFill();
			ofEnableAlphaBlending();
			ofSetColor(255,0,0,200);
			//ofRect(block.x+w,h*2+5,block.width,2);
			ofRect(block->x+w,h+block->y,block->width,block->height);
			ofDisableAlphaBlending();
			ofSetColor(255,255,255,255);
		}
	}
	
//		if(model->debugDetection)
//		{
//			ofFill();
//			ofSetColor(0, 0, 255);
//			ofRect(body.x+w,h*2+5,body.width,2);
//		}
//	}
	
	// merge seperated model->bodies
	model->mainBodies	= mergeBodies(model->mainBodies,50);
	model->bodies		= mergeBodies(model->bodies,20);
	
	
	// main body width check
	for(int i=0;i<model->mainBodies.size();i++)
	{
		ofRectangle *mainBody = model->mainBodies.at(i);
		if(mainBody->width < minMainBodyWidth)
		{
			model->mainBodies.erase(model->mainBodies.begin()+i);
			i--;
			continue;
		}
	}
	// body width check
	for(int i=0;i<model->bodies.size();i++)
	{
		ofRectangle *body = model->bodies.at(i);
		if(body->width < minBodyWidth)
		{
			model->bodies.erase(model->bodies.begin()+i);
			i--;
			continue;
		}
	}
	
	if(model->bodies.size() == 2)
	{
		model->player1.body = model->bodies.at(0);
		model->player2.body = model->bodies.at(1);
	}
	else
	{
		model->player1.body = NULL;
		model->player1.body = NULL;
	}
	
	if(model->mainBodies.size() == 2)
	{
		model->player1.mainBody = model->mainBodies.at(0);
		model->player2.mainBody = model->mainBodies.at(1);
	}
	else
	{
		model->player1.mainBody = NULL;
		model->player1.mainBody = NULL;
	}

	
	// gather block's data into BlockVO's, filter on distance to players
//	int maxBlockDisToPlayer = 100;
//	for(int i=0;i<model->blockVOs.size();i++)
//	{
//		delete model->blockVOs.at(i);
//	}
//	model->blockVOs.clear();
//	for(int i=0;i<model->blocks.size();i++)
//	{
//		ofRectangle *block = model->blocks.at(i);
//		BlockVO *blockVO = new BlockVO();
//		blockVO->bounds = *block;
//		
//		ofRectangle *player1MainBody = model->player1.mainBody;
//		ofRectangle *player2MainBody = model->player2.mainBody;
//		
//		int disToPlayer1 = (player1MainBody == NULL)? 9999 : block->x-(player1MainBody->x+player1MainBody->width);
//		int disToPlayer2 = (player2MainBody == NULL)? 9999 : player2MainBody->x-(block->x+block->width);
//		int disToPlayer;
//		if(disToPlayer1 < disToPlayer2)
//		{
//			disToPlayer = disToPlayer1;
//			blockVO->victim = 1;
//		}
//		else 
//		{
//			disToPlayer = disToPlayer2;
//			blockVO->victim = 2;
//			blockVO->mirrorX = true;
//		}
//		if(disToPlayer <= maxBlockDisToPlayer)
//		{
//			model->blockVOs.push_back(blockVO);
//		}
//	}
	
	// analyze blocks to find the real blocks
	// create blocks arrays for 2 players
	// loop trough blocks
	//   calculate distance
	//     if dis negative continue to next block
	//   determine owner
	//   check with highest dis to owner
	//     if higher store as highest
	// store fartest block per owner in playerVO's
	
	
	for(int i=0;i<model->blockVOs.size();i++)
	{
		delete model->blockVOs.at(i);
	}
	model->blockVOs.clear();
	
	int minBlockDisToPlayer = 0;
	int maxBlockDisToPlayer = 100;
	ofRectangle *player1MainBody = model->player1.mainBody;
	ofRectangle *player2MainBody = model->player2.mainBody;
	int farthestBlockDisPlayer1 = 0;
	int farthestBlockDisPlayer2 = 0;
	BlockVO *farthestBlockPlayer1 = NULL;
	BlockVO *farthestBlockPlayer2 = NULL;
	//cout << "check blocks\n";
	for(int i=0;i<model->blocks.size();i++)
	{
		ofRectangle *block = model->blocks.at(i);
		
		int disToPlayer1 = (player1MainBody == NULL)? 9999 : block->x-(player1MainBody->x+player1MainBody->width);
		int disToPlayer2 = (player2MainBody == NULL)? 9999 : player2MainBody->x-(block->x+block->width);
		//cout << "  disToPlayer1: " << disToPlayer1 << "\n";
		//cout << "  disToPlayer2: " << disToPlayer2 << "\n";
		
		int disToPlayer;
		
		BlockVO *blockVO = new BlockVO();
		blockVO->bounds = *block;
		
		if(disToPlayer1 < disToPlayer2)
		{
			disToPlayer = disToPlayer1;
			blockVO->victim = 1;
		}
		else 
		{
			disToPlayer = disToPlayer2;
			blockVO->victim = 2;
		}
		//cout << "  victim: " << blockVO->victim << "\n";
		
		if(disToPlayer <= maxBlockDisToPlayer && disToPlayer >= minBlockDisToPlayer)
		{
			//cout << "  checked\n";
			if(blockVO->victim == 1)
			{
				if(disToPlayer1 > farthestBlockDisPlayer1)
				{
					//cout << "  farthestBlock1\n";
					farthestBlockDisPlayer1 = disToPlayer1;
					farthestBlockPlayer1 = blockVO;	
				}
			}
			else if(blockVO->victim == 2)
			{
				if(disToPlayer2 > farthestBlockDisPlayer2)
				{
					//cout << "  farthestBlock2\n";
					farthestBlockDisPlayer2 = disToPlayer2;
					farthestBlockPlayer2 = blockVO;
				}
			}
			model->blockVOs.push_back(blockVO);
		}
	}
	model->player1.blockVO = farthestBlockPlayer1;
	//if(farthestBlockPlayer1 != NULL)
	//	cout << "  model->player1.blockVO.victim: " << model->player1.blockVO->victim << "\n";
	model->player2.blockVO = farthestBlockPlayer2;
	//if(farthestBlockPlayer2 != NULL)
	//	cout << "  model->player2.blockVO.victim: " << model->player2.blockVO->victim << "\n";
			
//		if(model->bodies.size() > 2)
//		{
//			vector<ofRectangle*> model->bodiesPoints;
//			for(int i=0;i<model->bodies.size();i++)
//				model->bodiesPoints.push_back(&model->bodies.at(i));
//			//std::sort(model->bodies.begin(), model->bodies.end(), &sortmodel->bodiesOnDissFromCenter);
//			std::sort(model->bodiesPoints.begin(), model->bodiesPoints.end(), &sortmodel->bodiesOnDissFromCenter);
//	//		for(int i=0;i<model->bodiesPoints.size();i++)
//	//		{
//	//			ofRectangle * body = model->bodiesPoints.at(i);
//	//			delete body;
//	//		}
//			model->bodiesPoints.clear();
//		}
//	//	if(model->bodies.size() > 1)
//	//	{
//	//		ofRectangle body1 = bodies.at(0);
//	//		ofRectangle body2 = bodies.at(1);
//	//		
//	//		if(body1.x < body2.x)
//	//		{
//	//			model->player1.body = body1;
//	//			model->player2.body = body2;
//	//		}
//	//		else 
//	//		{
//	//			model->player1.body = body2;
//	//			model->player2.body = body1;
//	//		}
//	//	}
//	//	else
//	//	{
//	//		model->player1.body.x = -1;
//	//		model->player1.body.x = -1;
//	//	}
	
	if(model->debugDetection)
	{
		ofFill();
		ofEnableAlphaBlending();
		ofSetColor(0, 0, 255,150);
		for(int i=0;i<model->bodies.size();i++)
		{
			ofRectangle *body = model->bodies.at(i);
			
			//ofRect(body->x+w,h*2+5+i*2,body->width,2);
			ofRect(body->x+w,h+body->y,body->width,body->height);
		}
		ofSetColor(255, 255, 0,150);
		for(int i=0;i<model->mainBodies.size();i++)
		{
			ofRectangle *mainBody = model->mainBodies.at(i);
			
			ofRect(mainBody->x+w,h*2+10+i*2,mainBody->width,2);
			//ofRect(mainBody->x+w,h+mainBody->y,mainBody->width,mainBody->height);
		}
		ofDisableAlphaBlending();
	}
	
	if(model->state != STATE_GAME)
	{
		detectPlayers();
	}
	
	model->checkBlocks();
	model->checkPlayersPositions();
}	

void VideoInputController::addBlock(int x, int y, int width, int height)
{			
	ofRectangle *block = new ofRectangle();
	block->x = x;
	block->y = y;
	block->width = width;
	block->height = height;

	model->blocks.push_back(block);
}
vector<ofRectangle*> VideoInputController::mergeBodies(vector<ofRectangle*> bodies, int maxMergeDis)
{			
	//int maxBodyMergeDis = 50; //50
	if(bodies.size() > 0)
		std::sort(bodies.begin(),bodies.end(), &sortRectsOnX);
	
	ofRectangle *prevBody = NULL;
	for(int i=0;i<bodies.size();i++)
	{
		ofRectangle *body = bodies.at(i);
		
		int dis = 0;
		if(prevBody != NULL)
		{
			dis = body->x-(prevBody->x+prevBody->width);
			if(dis < maxMergeDis)
			{
				body = mergeRectangles(body,prevBody);
				bodies[i] = body;
				bodies.erase(bodies.begin()+(i-1));
				i--;
			}
		}
		prevBody = body;
	}
	return bodies;
}
	
	
void VideoInputController::detectPlayers()
{
	//cout << "VideoInputController::detectPlayers\n";
	
	if(model->debugDetection)
	{
		int videoW = model->videoW;
		int videoH = model->videoH;
		ofRectangle *player1Area = model->player1.area;
		ofRectangle *player2Area = model->player2.area;
		
		ofNoFill();
		ofSetColor(0xcccc00);
		ofRect(player1Area->x, player1Area->y, player1Area->width, player1Area->height);
		ofRect(player2Area->x, player2Area->y, player2Area->width, player2Area->height);
		ofRect(videoW+player1Area->x, videoH+player1Area->y, player1Area->width, player1Area->height);
		ofRect(videoW+player2Area->x, videoH+player2Area->y, player2Area->width, player2Area->height);
	}
	
	bool detectedPlayer1 = false;
	bool detectedPlayer2 = false;
	for (int i = 0; i < model->bodies.size(); i++)
	{
		ofRectangle *body = model->bodies.at(i);
		
		//cout << "  i: " << i << " blobRect: " << blobRect << "\n";
		
		if(rectIsInside(body, model->player1.area, false))
			detectedPlayer1 = true;
		else if(rectIsInside(body, model->player2.area, false))
			detectedPlayer2 = true;
	}
	//cout << "  detectedPlayers: " << detectedPlayer1 << " & " << detectedPlayer2 << "\n";
	
	model->setDetectedPlayer1(detectedPlayer1);
	model->setDetectedPlayer2(detectedPlayer2);	
}
void VideoInputController::storeShadowBlobs()
{	
//	model->blobsHistory->push_back(model->currentBlobs);
//	if(model->blobsHistory->size() > model->maxBlobsHistoryLength)
//		model->blobsHistory->erase(model->blobsHistory->begin());
}
void VideoInputController::storeHistory()
{
	//cout << "VideoInputController::storeHistory\n";
	// store copy of hit blobs
	if(model->prevHitBlobs->size() > 0)
	{
		for (int i = 0; i < model->prevHitBlobs->size(); i++)
		{
			ofxCvBlob * blob = model->prevHitBlobs->at(i);
			delete blob;
		}
		model->prevHitBlobs->clear();
	}
    for (int i = 0; i < hitContourFinder.blobs.size(); i++)
	{
		ofxCvBlob blob = hitContourFinder.blobs[i];
		ofRectangle blobRect = blob.boundingRect;
		
		ofxCvBlob * blobCopy = new ofxCvBlob();
		blobCopy->boundingRect.x = blobRect.x;
		blobCopy->boundingRect.y = blobRect.y;
		blobCopy->boundingRect.width = blobRect.width;
		blobCopy->boundingRect.height = blobRect.height;
		//cout << "  new blob copy: " << blobCopy << "\n";
		model->prevHitBlobs->push_back(blobCopy);
	}
	
	// store pref grayimage
	model->prevGrayDiffImg->setFromPixels(model->grayDiffImg->getPixels(), model->videoW, model->videoH);
	
}

void VideoInputController::drawBlobsHistory()
{
	//cout << "VideoInputController::drawBlobsHistory\n";
	int blobsHistoryDisplayX = model->videoW*2;
	int blobsHistoryDisplayY = 0;
	
	model->prevGrayDiffImg->draw(blobsHistoryDisplayX,blobsHistoryDisplayY);
	
	int blobColors[4] = {0xff0000, 0x00ff00, 0x0000ff, 0xffff00};
	int historyLength = model->blobsHistory->size();
	if(historyLength > 0)
	{
		int initAlpha = 50;
		int alphaStep = (255-initAlpha)/historyLength;
		for (int i = 0; i < historyLength; i++)
		{
			vector<Blob*> * blobs = model->blobsHistory->at(i);
			
			if(i == 0)
				std::sort (blobs->begin(), blobs->end(), &sortBlobsOnX);
			
			for (int j = 0; j < blobs->size(); j++)
			{
				int alpha = initAlpha + alphaStep*i;
				Blob * blob = blobs->at(j);
				
				if(i == 0)
				{
					blob->color = blobColors[j];
				}
				else
				{
					vector<Blob*> * prevBlobs = model->blobsHistory->at(i-1);
					blob->color = 0xffffff;
					for (int l = 0; l < prevBlobs->size(); l++)
					{
						Blob * prevBlob = prevBlobs->at(l);
						if(matchBlobs(blob, prevBlob))
							blob->color = prevBlob->color;
					}
				}
				
				drawRect(blob->boundingRect,blobsHistoryDisplayX,blobsHistoryDisplayY,blob->color,alpha);
			}
		}
	}
	ofSetColor(0xffffff);
}
void VideoInputController::analyseHitBlobsSimple(unsigned char * colorPixels)
{
	//cout << "VideoInputController::analyseHitBlobsSimple\n";
	// per hit blob
	// check detection area
	// check unique
	// hittest check with current shadowblob
	// analyze left / right
	// correct with prev shadowblobs (if 2) 
	// human shadow hit filter ?
	// check hit blob speed
	
	int videoW = model->videoW;
	int videoH = model->videoH;
	
	int hitBlobDisplayX = 0;
	int hitBlobDisplayY = videoH;
	int attacksDisplayX = videoW*2;
	int attacksDisplayY = videoH;
	
	ofRectangle * detectionZone = &model->hitDetectionZone;
	if(model->debugDetection)
	{
		ofNoFill();
		ofSetColor(0xcc0000);
		ofRect(detectionZone->x, detectionZone->y, detectionZone->width, detectionZone->height);
		ofRect(hitBlobDisplayX+detectionZone->x, hitBlobDisplayY+detectionZone->y, detectionZone->width, detectionZone->height);
		
		int centerX = model->centerX;
		
		ofNoFill();
		ofSetColor(0xffffff);
		ofLine(centerX, 0, centerX, videoH);
		ofLine(hitBlobDisplayX+centerX, hitBlobDisplayY+0, hitBlobDisplayX+centerX, hitBlobDisplayY+videoH);
	}
	
	//cout << "hitContourFinder.blobs.size(): " << hitContourFinder.blobs.size() << "\n";
	
	if(hitContourFinder.blobs.size() == 0) return;
	
	model->hitsTextX = hitBlobDisplayX;
	model->hitsTextY = hitBlobDisplayY;
	
	//drawHitText("  Found hit blobs ");
	
	model->hitCounter = 0;
	for (int i = 0; i < hitContourFinder.blobs.size(); i++)
	{
		ofxCvBlob hitBlob = hitContourFinder.blobs[i];
		ofRectangle hitBlobRect = hitBlob.boundingRect;
		
		model->hitCounter++;

		if(model->debugDetection)
		{
			//drawHitText("  Found hit blob");
			hitBlob.draw(0,0);
			hitBlob.draw(hitBlobDisplayX,hitBlobDisplayY);
		}
		
		/*if(!isCorrectColor(hitBlobRect,colorPixels))
		{
			drawHitText("  Hit blob doesn't have the right color");
			takeHitScreenShot("("+ofToString(model->hitCounter)+")");
			continue;
		}	
		drawHitText("  Hit blob has the right color");*/
		
		int hitBlobXC = hitBlobRect.x+hitBlobRect.width/2;
		int hitBlobYC = hitBlobRect.y+hitBlobRect.height/2;
		if(!(hitBlobXC >= detectionZone->x &&
			 hitBlobYC >= detectionZone->y &&
			 hitBlobXC <= detectionZone->x+detectionZone->width &&
			 hitBlobYC <= detectionZone->y+detectionZone->height))
		{
			//drawHitText("  Hit blob is outside detectionZone");
			continue;
		}
		drawHitText("  Found hit blobs ");
		drawHitText("  Hit blob is inside detectionZone");
		
		if(model->player1.body == NULL || model->player2.body == NULL)
		{
			drawHitText("  There are not 2 body's detected");
			takeHitScreenShot("("+ofToString(model->hitCounter)+")");
			continue;
		}
		drawHitText("  There are 2 bodies");
		
		if(!hitIsUnique(hitBlobRect))
		{
			drawHitText("  Hit blob isn't a unique hit blob");
			takeHitScreenShot("("+ofToString(model->hitCounter)+")");
			continue;
		}
		drawHitText("  Hit blob is a unique hit blob");
		
		
//		// hittest with current shadow blobs (still needed?)
//		bool hitsShadow = false;
//		for (int j = 0; j < contourFinder.blobs.size(); j++)
//		{
//			ofxCvBlob shadowBlob = contourFinder.blobs[j];
//			ofRectangle shadowBlobRect = shadowBlob.boundingRect;
//			if(rectHitTest(hitBlobRect,shadowBlobRect))
//				hitsShadow = true;
//		}
//		
//		if(!hitsShadow)
//		{
//			drawHitText("  Hit blob doesn't hit shadow blobs");
//			takeHitScreenShot("("+ofToString(model->hitCounter)+")");
//			continue;
//		}	
//		drawHitText("  Hit blob hits shadow blob(s)");
		
		
		
		
		
		
		
		
		
		/*int hitBlobRectCX = hitBlobRect.x+hitBlobRect.width/2;
		int deadZoneWidth = 20;
		int deadZoneL = model->centerX-deadZoneWidth/2;
		int deadZoneR = model->centerX+deadZoneWidth/2;
		
		if(hitBlobRectCX > deadZoneL && hitBlobRectCX < deadZoneR)
		{
			drawHitText("  Hit blob inside deadzone");
			takeHitScreenShot("("+ofToString(model->hitCounter)+")");
			continue;
		}
		drawHitText("  Hit blob outside deadzone");
		
		if(!hitsBodyPixels(hitBlobRect))
		{
			drawHitText("  Hit blob doesn't hit the body");
			takeHitScreenShot("("+ofToString(model->hitCounter)+")");
			continue;
		}
		drawHitText("  Hit blob hit's the body");
		   
		int victim = (hitBlobRectCX < model->centerX)? 1 : 2;*/
		
		
		

//		int hitBlobRectCX = hitBlobRect.x+hitBlobRect.width/2;
//		ofRectangle *bodyP1 = model->player1.body;
//		ofRectangle *bodyP2 = model->player2.body;
//		
//		string hitText = "  ";
//		hitText += "hit: " + ofToString(hitBlobRectCX,1) + "|";
//		hitText += "1: " + ofToString(bodyP1->x,1) + " >< " + ofToString(bodyP1->x+bodyP1->width,1) + "|";
//		hitText += "2: " + ofToString(bodyP2->x,1) + " >< " + ofToString(bodyP2->x+bodyP2->width,1);
//		
//		drawHitText(hitText);
//		
//		int victim = 0;	
//		if(hitBlobRectCX > bodyP1->x && hitBlobRectCX < bodyP1->x+bodyP1->width)
//			victim = 1;			
//		else if(hitBlobRectCX > bodyP2->x && hitBlobRectCX < bodyP2->x+bodyP2->width)
//			victim = 2;
		
		
		int hitBlobRectL = hitBlobRect.x;
		int hitBlobRectR = hitBlobRect.x+hitBlobRect.width;
		ofRectangle *bodyP1 = model->player1.body;
		ofRectangle *bodyP2 = model->player2.body;
		
		
		bool hittingPlayer1 = false;
		if((hitBlobRectL > bodyP1->x && hitBlobRectL < bodyP1->x+bodyP1->width) ||
		   (hitBlobRectR > bodyP1->x && hitBlobRectR < bodyP1->x+bodyP1->width))
			hittingPlayer1 = true;
		bool hittingPlayer2 = false;
		if((hitBlobRectL > bodyP2->x && hitBlobRectL < bodyP2->x+bodyP2->width) ||
		   (hitBlobRectR > bodyP2->x && hitBlobRectR < bodyP2->x+bodyP2->width))
			hittingPlayer2 = true;
		
		if((hittingPlayer1 == true && hittingPlayer2 == true) ||
		   (hittingPlayer1 == false && hittingPlayer2 == false))
		{
			drawHitText("  Can't determine victim");
			takeHitScreenShot("("+ofToString(model->hitCounter)+")");
			continue;
		}
		int victim = (hittingPlayer1)? 1 : 2; 
		drawHitText("  victim: "+ofToString(victim));
		
//		if(!hitsBodyPixels(hitBlobRect)) // still really needed?
//		{
//			drawHitText("  Hit blob doesn't hit the body pixels");
//			takeHitScreenShot("("+ofToString(model->hitCounter)+")");
//			continue;
//		}
		
			
		//  hitsBodyPixels();
		
		
		
		
		
		
		/*int attackSpeed = 0;
		int numPrevHitBlobs = model->prevHitBlobs->size();
		for (int j = 0; j < numPrevHitBlobs; j++)
		{
			ofxCvBlob* prevHitBlob = model->prevHitBlobs->at(j);
			if(matchBlobs(&hitBlob, prevHitBlob))
			{
				ofRectangle prevHitBlobRect = prevHitBlob->boundingRect;
				attackSpeed = hitBlobRect.x-prevHitBlobRect.x;
				drawHitText("  attackSpeed: " + ofToString(attackSpeed));
			}
			//ofRectangle prevHitBlobRect = prevHitBlob->boundingRect;
			//int prevHitBlobRect.x
			
			cout << "  attackSpeed: " << attackSpeed << "\n";
			
			// if hit blobs are to close to a prev hit blob they are not unique
		}
		if(attackSpeed > -5 && victim == 1)
			continue;
		else if(attackSpeed < 5 && victim == 2)
			continue;*/
		
		
//		bool isBlocked = false;
//		int maxBlockDisX = model->maxBlockDisX;
//		//cout << "VideoInputController::isBlocked\n";
//		int hitCY = hitBlobRect.y+hitBlobRect.height/2;
//		int hitCX = hitBlobRect.x+hitBlobRect.width/2;
//		//cout << "hitCY: " << hitCY << "\n";
//		for(int i=0;i<model->blocks.size();i++)
//		{
//			ofRectangle *block = model->blocks.at(i);
//			
//			int disX = block->x - hitCX;
//			
//			//cout << "block: " << block->y << " t/m " << block->y+block->height << "\n";
//			if(hitCY > block->y && hitCY < block->y+block->height && 
//			   disX < maxBlockDisX && disX > -maxBlockDisX)
//			{
//				cout << "  isBlocked\n";
//				
//				BlockVO blockVO;
//				blockVO.bounds = *block;
//				blockVO.victim = victim;
//				blockVO.mirrorX = (disX < 0);
//				model->block(blockVO);
//				
//				isBlocked = true;
//				break;
//			}
//		}
		
		bool isBlocked = false;
		//cout << "VideoInputController::isBlocked\n";
		int hitCY = hitBlobRect.y+hitBlobRect.height/2;
		int hitCX = hitBlobRect.x+hitBlobRect.width/2;
		//cout << "hitCY: " << hitCY << "\n";
		BlockVO *blockVO = NULL;
		if(victim == 1)
			blockVO = model->player1.blockVO;
		else if(victim == 2)
			blockVO = model->player2.blockVO;
		
		if(blockVO != NULL)
		{
			ofRectangle *block = &blockVO->bounds;
			if(hitCY > block->y && hitCY < block->y+block->height)
			{
				isBlocked = true;
				model->block(blockVO);
			}
		}
		if(!isBlocked)
		{
			cout << "  isHit\n";
			// TODO: correct with prev shadowblobs (if 2) 
			// TODO: human shadow hit filter ?
			// TODO: check hit blob speed (find old, match blobs, check speed) 
			//			(store if hit blobs are valid, store only valid blobs)

			HitVO hitVO;
			hitVO.bounds = hitBlobRect; //new ofRectangle();
			hitVO.victim = victim;
			drawHitText("  victim: "+ofToString(victim));		
			model->hit(hitVO);
			
			drawHitText("  healths: "+ofToString(model->player1Health)+" vs. "+ofToString(model->player2Health));
			takeHitScreenShot("("+ofToString(model->hitCounter)+")");
		}
	}
}
//void VideoInputController::analyseHitBlobs()
//{
//	//cout << "VideoInputController::analyseHitBlobs (frame: "<<model->frameCounter<<")\n";
//	
//	if(hitContourFinder.blobs.size() == 0) return;
//	
//	int hitBlobDisplayX = 0;
//	int hitBlobDisplayY = model->videoH;
//	int attacksDisplayX = model->videoW*2;
//	int attacksDisplayY = model->videoH;
//	
//	model->hitsTextX = hitBlobDisplayX;
//	model->hitsTextY = hitBlobDisplayY;
//	
//	
//	drawHitText("  Found hit blobs ");
//	
//	ofRectangle * detectionZone = &model->hitDetectionZone;
//	if(model->debugDetection)
//	{
//		ofNoFill();
//		ofSetColor(0xcc0000);
//		ofRect(detectionZone->x, detectionZone->y, detectionZone->width, detectionZone->height);
//		ofRect(hitBlobDisplayX+detectionZone->x, hitBlobDisplayY+detectionZone->y, detectionZone->width, detectionZone->height);
//	}
//	int validBlobs = 0;
//	for (int i = 0; i < hitContourFinder.blobs.size(); i++)
//	{
//		ofxCvBlob hitBlob = hitContourFinder.blobs[i];
//		ofRectangle hitBlobRect = hitBlob.boundingRect;
//		
//		if(!(hitBlobRect.x >= detectionZone->x &&
//			 hitBlobRect.y >= detectionZone->y &&
//			 hitBlobRect.x+hitBlobRect.width <= detectionZone->x+detectionZone->width &&
//			 hitBlobRect.y+hitBlobRect.height <= detectionZone->y+detectionZone->height))
//		{
//			validBlobs++;
//		}
//	}
//	if(validBlobs <= 0)
//	{
//		drawHitText("  0 hit blobs inside detection area.");
//		return;
//	}
//	
//	// if there where not 2 blobs before this frame, we can't check for an attack, so cancel
//	vector<vector<Blob*> * > * blobsHistory = model->blobsHistory;
//	int historyLength = blobsHistory->size();
//	if(blobsHistory->size() < 1) 
//	{
//		drawHitText("  There is not enough blob history to detect attacks");
//		takeHitScreenShot("("+ofToString(model->hitCounter)+")");
//		return;
//	}
//	drawHitText("  There is enough blob history to detect attacks");
//
//	vector<Blob*> * prevBlobs = blobsHistory->at(blobsHistory->size()-1);
//	if(prevBlobs->size() < 2)
//	{
//		drawHitText("  There weren't 2 blobs in prev blobs\n");
//		takeHitScreenShot("("+ofToString(model->hitCounter)+")");
//		return;
//	}
//	drawHitText("  There were 2 blobs in prev blobs\n");
//	
//	model->hitCounter = 0;
//	for (int i = 0; i < hitContourFinder.blobs.size(); i++)
//	{
//		ofxCvBlob hitBlob = hitContourFinder.blobs[i];
//		ofRectangle hitBlobRect = hitBlob.boundingRect;
//		
//		model->hitCounter++;
//		model->attacksTextX = attacksDisplayX;
//		model->attacksTextY = attacksDisplayY;
//		
//		if(model->debugDetection)
//		{
//			drawHitText("  Found hit blob");
//			hitBlob.draw(0,0);
//			hitBlob.draw(hitBlobDisplayX,hitBlobDisplayY);
//		}
//		
//		if(!(hitBlobRect.x >= detectionZone->x &&
//			 hitBlobRect.y >= detectionZone->y &&
//			 hitBlobRect.x+hitBlobRect.width <= detectionZone->x+detectionZone->width &&
//			 hitBlobRect.y+hitBlobRect.height <= detectionZone->y+detectionZone->height))
//		{
//			drawHitText("  Hit blob is outside detectionZone");
//			continue;
//		}
//		drawHitText("  Hit blob is inside detectionZone");
//		
//		if(!hitIsUnique(hitBlobRect))
//		{
//			drawHitText("  Hit blob isn't a unique hit blob");
//			takeHitScreenShot("("+ofToString(model->hitCounter)+")");
//			continue;
//		}
//		drawHitText("  Hit blob is a unique hit blob");
//		
//		//cout << "VideoInputController::there was an attack? (" << model->possibleAttacksCounter << ")\n";
//		
//		// Analyze possible attack
//		string counters = ofToString(model->hitCounter)+"/"+ofToString(model->possibleAttacksCounter);
//		
//		//int blobDiffTolerance = 100;
//		int minAttackSpeed = model->minAttackSpeed;
//		//cout << "  minAttackSpeed: " << minAttackSpeed << "\n";
//		bool isAttack = false;
//		
//		drawAttackText("there was an attack? ("+counters+")");
//		
//		ofSetColor(0xffffff);
//		if(model->debugDetection)
//			model->prevGrayDiffImg->draw(attacksDisplayX,attacksDisplayY);
//		
//		int historyLength = model->blobsHistory->size();
//		if(historyLength < 2) 
//		{
//			drawAttackText("  not enough blobs history");
//			takeHitScreenShot("("+ofToString(model->hitCounter)+")");
//			return; 
//		}
//		drawAttackText("  enough blobs history");
//		
//		vector<Blob*> * prevBlobs = model->blobsHistory->at(historyLength-1);
//		std::sort (prevBlobs->begin(), prevBlobs->end(), &sortBlobsOnX);
//		
//		int blobColors[4] = {0xff0000, 0x00ff00, 0x0000ff, 0xffff00};
//		for (int i = 0; i < prevBlobs->size(); i++)
//		{
//			Blob * prevBlob = prevBlobs->at(i);
//			ofRectangle prevBlobRect = prevBlob->boundingRect;
//			prevBlob->color = blobColors[i];
//			
//			vector<Blob*> * prevPrevBlobs = model->blobsHistory->at(historyLength-2);
//			if(prevPrevBlobs->size() < 2) 
//			{
//				drawAttackText("  not enough prev prev blobs");
//				takeHitScreenShot("("+ofToString(model->hitCounter)+")");
//			}
//			for (int j = 0; j < prevPrevBlobs->size(); j++)
//			{
//				Blob * prevPrevBlob = prevPrevBlobs->at(j);
//				if(matchBlobs(prevBlob, prevPrevBlob))
//				{
//					ofRectangle prevPrevBlobRect = prevPrevBlob->boundingRect;
//					prevPrevBlob->color = prevBlob->color;
//					drawRect(prevPrevBlob->boundingRect,attacksDisplayX,attacksDisplayY,prevPrevBlob->color,200);
//					
//					drawAttackText("  found matching blob to detect speed");
//					
//					int attackSpeed;
//					bool onLeftSide = (prevBlobRect.x+prevBlobRect.width/2 < model->videoW/2);
//					int player = (onLeftSide)?1:2;
//					drawAttackText("  player: "+ofToString(player));
//					if(onLeftSide)
//						attackSpeed = (prevBlobRect.x+prevBlobRect.width)-(prevPrevBlobRect.x+prevPrevBlobRect.width);
//					else 
//						attackSpeed = (prevPrevBlobRect.x-prevBlobRect.x);
//					
//					drawAttackText("  attackSpeed: "+ofToString(attackSpeed));
//					
//					if(attackSpeed >= minAttackSpeed)
//					{
//						drawHitText("  Is attack");
//						
//						// analyze who's the victim
//						
//						int victim = (onLeftSide)? 2 : 1;
//						drawAttackText("  victim: "+ofToString(victim));
//						
//						Blob * victimBlob = prevBlobs->at(victim-1);
//						ofRectangle victimBlobRect = victimBlob->boundingRect;
//						
//						hitBlob.draw(attacksDisplayX,attacksDisplayY);
//						
//						if(rectHitTest(hitBlobRect,victimBlobRect))
//						{
//							if(model->debugDetection)
//								drawRect(victimBlobRect, attacksDisplayX,attacksDisplayY, 0xffff00);	
//							drawAttackText("  Hit!");
//							drawHitText("  Hit hits a player");
//							
//							model->hitRect->x = hitBlobRect.x;
//							model->hitRect->y = hitBlobRect.y;
//							model->hitRect->width = hitBlobRect.width;
//							model->hitRect->height = hitBlobRect.height;	
//							
//							model->hit(0,0,victim);
//							
//							drawHitText("  healths: "+ofToString(model->player1Health)+" vs. "+ofToString(model->player2Health));
//							takeHitScreenShot("("+ofToString(model->hitCounter)+")");
//						}
//						else
//						{
//							drawAttackText("  no hit");
//							takeHitScreenShot("("+ofToString(model->hitCounter)+")");
//						}
//					}
//					else 
//					{
//						drawHitText("  Is not attack");
//						takeHitScreenShot("("+ofToString(model->hitCounter)+")");
//					}
//
//				}
//				else
//				{
//					drawAttackText("  not a matching blob to detect speed");	
//				}
//			}
//			drawRect(prevBlob->boundingRect,attacksDisplayX,attacksDisplayY,prevBlob->color,255);
//		}	
//		model->possibleAttacksCounter++;
//	}
//}
bool VideoInputController::hitIsUnique(ofRectangle blobRect)
{
	//cout << "VideoInputController::hitIsUnique\n";
	bool toCloseToPrevHit = false;
	//cout << "  model->prevHitBlobs->size(): " << model->prevHitBlobs->size() << "\n";
	//cout << "  model->minDiffHitBlobsPos: " << model->minDiffHitBlobsPos << "\n";
	
	int numPrevHitBlobs = model->prevHitBlobs->size();
	for (int j = 0; j < numPrevHitBlobs; j++)
	{
		ofxCvBlob* prevBlob = model->prevHitBlobs->at(j);
		ofRectangle prevBlobRect = prevBlob->boundingRect;
		
		float dist = ofDist(prevBlobRect.x, prevBlobRect.y, 
							blobRect.x, blobRect.y);
		
		//cout << "  dist: " << dist << "\n";
		
		// if hit blobs are to close to a prev hit blob they are not unique
		if(dist < model->minDiffHitBlobsPos) 
			toCloseToPrevHit = true;
	}
	//cout << "  toCloseToPrevHit: " << toCloseToPrevHit << "\n";
	
	return !toCloseToPrevHit;
}
bool VideoInputController::hitsBodyPixels(ofRectangle hitBlobRect)
{
	//cout << "VideoInputController::hitsBodyPixels\n";
	int videoW = model->videoW;
	int videoH = model->videoH;
	int minNumWhitePixels = videoH*0.10;
	
	int hitX = hitBlobRect.x+hitBlobRect.width/2;
	
	int numWhitePixels = 0;
	unsigned char * pixels = model->grayDiffImg->getPixels();
	for (int i = 0; i < videoH; i++) 
	{
		int pixelIndex = (i * videoW) + hitX;
		if(pixels[pixelIndex] == 255)
			numWhitePixels++;
		else
			pixels[pixelIndex] = 200;
	}
	model->grayDiffImg->setFromPixels(pixels, videoW, videoH);
	if(model->debugDetection)
	{
		ofSetColor(0xffffff);
		model->grayDiffImg->draw(model->videoW, model->videoH);
	}
	
	return (numWhitePixels >= minNumWhitePixels);
}
/*bool VideoInputController::isBlocked(ofRectangle hitBlobRect,vector<ofRectangle*> blocks)
{
	int maxBlockDisX = model->maxBlockDisX;
	//cout << "VideoInputController::isBlocked\n";
	int hitCY = hitBlobRect.y+hitBlobRect.height/2;
	int hitCX = hitBlobRect.x+hitBlobRect.width/2;
	//cout << "hitCY: " << hitCY << "\n";
	for(int i=0;i<blocks.size();i++)
	{
		ofRectangle *block = blocks.at(i);
		
		int disX = hitCX - block->x;
		if(disX < 0) disX *= -1;
		
		//cout << "block: " << block->y << " t/m " << block->y+block->height << "\n";
		if(hitCY > block->y && hitCY < block->y+block->height && disX < maxBlockDisX)
			return true;
	}
	return false;
}*/
bool VideoInputController::isCorrectColor(ofRectangle hitBlobRect, unsigned char * colorPixels)
{
	int videoW = model->videoW;
	int videoH = model->videoH;
	
	unsigned char * diffPixels = model->grayHitDiffImg->getPixels();
	
	int totalR = 0;
	int totalG = 0;
	int totalB = 0;
	
	ofxCvColorImage colorImg;
	colorImg.allocate(hitBlobRect.width, hitBlobRect.height);
	//unsigned char * newPixels = new unsigned char[videoW*videoH*3];
	unsigned char * newPixels = colorImg.getPixels();
	int newPixelIndex = 0;
	for (int i = hitBlobRect.y; i < hitBlobRect.y+hitBlobRect.height; i++) 
	{
		for(int j = hitBlobRect.x; j < hitBlobRect.x+hitBlobRect.width; j++) 
		{
			int pixelsIndex = (i * videoW) + j;
			
			if(diffPixels[pixelsIndex] == 255)
			{
				pixelsIndex *= 3; // color
				int r = colorPixels[pixelsIndex];
				totalR += r;
				int g = colorPixels[pixelsIndex+1];
				totalG += g;
				int b = colorPixels[pixelsIndex+2];
				totalB += b;
				
				newPixels[newPixelIndex] = r;
				newPixelIndex++;
				newPixels[newPixelIndex+1] = b;
				newPixelIndex++;
				newPixels[newPixelIndex+2] = g;
				newPixelIndex++;
			}
			else
			{
				pixelsIndex *= 3; // color
				if(model->debugDetection)
				{
					newPixels[newPixelIndex] = 0;
					newPixelIndex++;
					newPixels[newPixelIndex] = 0;
					newPixelIndex++;
					newPixels[newPixelIndex] = 0;
					newPixelIndex++;
				}
			}
			
		}
	}
	if(model->debugDetection)
	{
		ofSetColor(255, 255, 255);
		colorImg.setFromPixels(newPixels, hitBlobRect.width, hitBlobRect.height);
		colorImg.draw(videoW+hitBlobRect.x,hitBlobRect.y);
		
		
		int x = videoW+hitBlobRect.x;
		int y = hitBlobRect.y+hitBlobRect.height;
		int w = 5;
		int h;
		int spacing = 1;
		int numPixels = hitBlobRect.width*hitBlobRect.height;
		//r
		int r = totalR/numPixels;
		h = float(r)/255*20;
		ofSetColor(255, 0, 0);
		ofRect(x,y-h,w,h);
		//g
		int g = totalG/numPixels;
		h = float(g)/255*20;
		ofSetColor(0, 255, 0);
		ofRect(x+w+spacing,y-h,w,h);
		//b
		int b = totalB/numPixels;
		h = float(b)/255*20;
		ofSetColor(0, 0, 255);
		ofRect(x+w*2+spacing*2,y-h,w,h);
		
		
		
	}
	return true;
}
void VideoInputController::drawHitText(string text)
{
	if(!model->debugDetection) return;
	//cout << "drawHitText: " << text << "\n";
	ofSetColor(0x0000ff);
	font.drawString(text, model->hitsTextX, model->hitsTextY+=model->lineHeight);
}
void VideoInputController::drawAttackText(string text)
{
	if(!model->debugDetection) return;
	ofSetColor(0x0000ff);
	font.drawString(text, model->attacksTextX, model->attacksTextY+=model->lineHeight);
}

//void VideoInputController::analyzeShadowsForPlayers()
//{
//	//cout << "VideoInputController::analyzeShadowsForPlayers\n";
//
//	if(model->debugDetection)
//	{
//		int videoW = model->videoW;
//		int videoH = model->videoH;
//		ofRectangle *player1Area = model->player1.area;
//		ofRectangle *player2Area = model->player2.area;
//		
//		ofNoFill();
//		ofSetColor(0xcccc00);
//		ofRect(player1Area->x, player1Area->y, player1Area->width, player1Area->height);
//		ofRect(player2Area->x, player2Area->y, player2Area->width, player2Area->height);
//		ofRect(videoW+player1Area->x, videoH+player1Area->y, player1Area->width, player1Area->height);
//		ofRect(videoW+player2Area->x, videoH+player2Area->y, player2Area->width, player2Area->height);
//	}
//	
//	bool detectedPlayer1 = false;
//	bool detectedPlayer2 = false;
//    for (int i = 0; i < contourFinder.blobs.size(); i++)
//	{
//
//		ofxCvBlob blob = contourFinder.blobs[i];
//		ofRectangle * blobRect = &blob.boundingRect;
//		
//		//cout << "  i: " << i << " blobRect: " << blobRect << "\n";
//		
//		if(rectIsInside(blobRect, model->player1.area, false))
//			detectedPlayer1 = true;
//		else if(rectIsInside(blobRect, model->player2.area, false))
//			detectedPlayer2 = true;
//	}
//	//cout << "  detectedPlayers: " << detectedPlayer1 << " & " << detectedPlayer2 << "\n";
//	
//	model->setDetectedPlayer1(detectedPlayer1);
//	model->setDetectedPlayer2(detectedPlayer2);
//}

// -- utils --
unsigned char * VideoInputController::toGrayscale(unsigned char * pixels)
{
	//cout << "VideoInputController::toGrayscale\n";
	int videoW = model->videoW;
	int videoH = model->videoH;
	unsigned char * grayPixels = new unsigned char[videoW*videoH];
	int counter = 0;
	for (int i = 0; i < videoH; i++) {
		for (int j = 0; j < videoW*3; j+=3) {
			
			// get r, g and b components
			int r = (i*videoW*3) + j;
			int g = (i*videoW*3) + (j+1);
			int b = (i*videoW*3) + (j+2);
			
			int grayPixel = (11 * pixels[r] + 16 * pixels[g] + 5 * pixels[b]) / 32;
			
			grayPixels[counter] = grayPixel;
			
			counter++;
		}
	}
	return grayPixels;
}
void VideoInputController::drawRect(ofRectangle rect,int x, int y, int color, int a)
{
	ofEnableAlphaBlending();
	ofNoFill();
	int r = (color >> 16) & 0xff;
	int g = (color >> 8) & 0xff;
	int b = (color >> 0) & 0xff;
	ofSetColor(r,g,b,a);	
	ofRect(x+rect.x, 
		   y+rect.y, 		   
		   rect.width, 
		   rect.height);
	ofSetColor(0xffffff);
	ofDisableAlphaBlending();

}
void VideoInputController::drawRect(ofRectangle rect,int x, int y, int color)
{
	drawRect(rect, x, y, color, 255);
}
void VideoInputController::takeScreenShot(string extraText)
{
	if(!model->takeScreenShots) return;
	cout << "VideoInputController::takeScreenShot " << extraText << "\n";
	string frameCounter = ofToString(model->frameCounter);
	string fileName = "screenshots/frame"+frameCounter+" "+extraText+".png";
	//ofSaveScreen(fileName);
	imageSaver.grabScreen(0,0,ofGetWidth(),ofGetHeight());
	imageSaver.saveThreaded(fileName);
}
void VideoInputController::takeHitScreenShot(string extraText)
{
	if(!model->takeHitScreenShots) return;
	string frameCounter = ofToString(model->frameCounter);
	string fileName = "screenshots/frame"+frameCounter+" (hit) "+extraText+".png";
	//ofSaveScreen(fileName);
	imageSaver.grabScreen(0,0,ofGetWidth(),ofGetHeight());
	imageSaver.saveThreaded(fileName);
}
bool VideoInputController::matchBlobs(ofxCvBlob * blob1, ofxCvBlob * blob2)
{
	//cout << "VideoInputController::matchBlobs\n";
	ofRectangle blobRect1 = blob1->boundingRect;
	ofRectangle blobRect2 = blob2->boundingRect;
	
	/*int blob1CX = blobRect1.x+blobRect1.width/2;
	int blob1CY = blobRect1.y+blobRect1.height/2;
	int blob2CX = blobRect2.x+blobRect2.width/2;
	int blob2CY = blobRect2.y+blobRect2.height/2;
	
	int dis = ofDist(blob1CX,blob1CY,blob2CX,blob2CY);*/
	
	int blob1CX = blobRect1.x+blobRect1.width/2;
	int blob2CX = blobRect2.x+blobRect2.width/2;
	int dis = blob2CX-blob1CX;
	
	if(dis < 0) dis *= -1;
	
	bool match = (dis < model->blobDiffTolerance);
	return match;
}
bool VideoInputController::rectHitTest(ofRectangle rect1,ofRectangle rect2)
{
	if(rect1.y					<	rect2.y+rect2.height &&
	   rect1.y+rect1.height		>	rect2.y &&
	   rect1.x					<	rect2.x+rect2.width &&
	   rect1.x+rect1.width		>	rect2.x)
		return true;
	else
		return false;
}
void VideoInputController::colorInImage(ofxCvGrayscaleImage * image, int color, ofRectangle rect)
{
	int width = image->getWidth();
	int height = image->getHeight();
	unsigned char * orgPixels = image->getPixels();
	//int newPixelsIndex = 0;
	for (int i = rect.y; i < rect.y+rect.height; i++) 
	{
		for(int j = rect.x; j < rect.x+rect.width; j++) 
		{
			int orgPixelsIndex = (i * width) + j;				
			//orgPixels[orgPixelsIndex] = newPixels[newPixelsIndex];
			orgPixels[orgPixelsIndex] = color;
		}
	}
	image->setFromPixels(orgPixels, width, height);
	
	//delete [] orgPixels;
}
ofRectangle * VideoInputController::mergeRectangles(ofRectangle *rect1, ofRectangle *rect2)
{
	ofRectangle * mergedRect = new ofRectangle();
	mergedRect->x = (rect1->x < rect2->x)? rect1->x : rect2->x;
	mergedRect->y = (rect1->y < rect2->y)? rect1->y : rect2->y;
	
	int right1 = rect1->x+rect1->width;
	int right2 = rect2->x+rect2->width;
	int right = (right1 < right2)? right2 : right1;
	mergedRect->width = right-mergedRect->x;
	
	int bottom1 = rect1->y+rect1->height;
	int bottom2 = rect2->y+rect2->height;
	int bottom = (bottom1 < bottom2)? bottom2 : bottom1;
	mergedRect->height = bottom-mergedRect->y;
	
	return mergedRect;
}
bool VideoInputController::rectIsInside(ofRectangle *smallRect, ofRectangle *bigRect, bool completelyInside)
{
	if(completelyInside)
	{
		return ((smallRect->x >= bigRect->x &&
				 smallRect->y >= bigRect->y &&
				 smallRect->x+smallRect->width <= bigRect->x+bigRect->width &&
				 smallRect->y+smallRect->height <= bigRect->y+bigRect->height));
	}
	else 
	{
		int rectCX = smallRect->x+smallRect->width/2;
		int rectCY = smallRect->y+smallRect->height/2;
		
		return ((rectCX >= bigRect->x &&
				 rectCY >= bigRect->y &&
				 rectCX <= bigRect->x+bigRect->width &&
				 rectCY <= bigRect->y+bigRect->height));
	}
}