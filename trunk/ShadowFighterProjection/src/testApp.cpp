#include "testApp.h"

#define HOST			"localhost"
#define PORT			12345
#define HIT_ADDRESS		0
#define BOUNDING_BOX	1
#define HEALTH			2
#define WINNER			3
#define STATE			4
#define PLAYERS_UPDATE	5
#define COUNT_DOWN		6
#define BLOCK			7
#define BLOCKING		8
#define POWERHIT_UPDATE 9

//--------------------------------------------------------------
void testApp::setup()
{
	videoW				= 640;
	videoH				= 460;
	screenW				= 1024;
	screenH				= 768;
	
	debug				= false;
	disableDraw			= false;
	lockState			= false;
	state				= -1;
	detectedPlayer1		= false;
	detectedPlayer2		= false;
	
	ofSetFrameRate(31);
	ofBackground(0,0,0);
	ofSetCircleResolution(100);
	
	int marginX = (screenW-healthBarPlayer1.width-healthBarPlayer2.width)/3;
	healthBarPlayer1.x = marginX;
	healthBarPlayer2.x = screenW-healthBarPlayer2.width-marginX;
	healthBarPlayer1.y = healthBarPlayer2.y = 20;
	healthBarPlayer1.mirrorX = false;
	healthBarPlayer2.mirrorX = true;
	loadData();
	
	powerhitPlayer1 = false;
	powerhitPlayer2 = false;
	powerhit1Img.loadImage("images/powerhit/powerhit1.png");
	powerhit2Img.loadImage("images/powerhit/powerhit2.png");
	
	setState(STATE_DEMO);
	
	receiver.setup(PORT);
}

void testApp::loadData()
{
	if( xml.loadFile("config.xml") ){
		cout << "config.xml loaded\n";
	}else{
		cout << "Unable to load config.xml check data folder\n";
	}
	parseXML();
	storeValues();
	
}
void testApp::parseXML()
{
	x				= xml.getValue("x", 1.0);
	y				= xml.getValue("y", 1.0);
	alpha			= xml.getValue("alpha", 1.0);
	scale			= xml.getValue("scale", 1.00);
	debug			= xml.getValue("debug", false);
	
	cout << "x: " << x << "\n";
	cout << "y: " << y << "\n";
	cout << "scale: " << scale << "\n";
	cout << "alpha: " << alpha << "\n";
	cout << "debug: " << debug << "\n";
}
void testApp::storeValues()
{
	xml.setValue("x", x);
	xml.setValue("y", y);
	xml.setValue("scale", scale);
	xml.setValue("alpha", alpha);
	xml.setValue("debug", debug);
	xml.saveFile("config.xml");
}


void testApp::addHit(float hitX,float hitY, float hitW, float HitH, int hitType)
{
	cout << "testApp::addHit\n";
	cout << "  hitType: "<<hitType<<"\n";
	hitX = hitX+hitW/2;
	hitY = hitY+HitH/2;
	
	HitIndicator *hitIndicator = new HitIndicator(hitType);
	hitIndicator->x = hitX/videoW*screenW*scale+x;
	hitIndicator->y = hitY/videoH*screenH*scale+y;
	hitIndicator->scale = scale;
	hitIndicator->alpha = alpha;
	effects.addChild(hitIndicator);
}
void testApp::addBlock(float blockX,float blockY, float blockW, float blockH, float blockVictim)
{
	blockX = blockX+blockW/2;
	blockY = blockY+blockH/2;
	
	BlockIndicator *blockIndicator = new BlockIndicator();
	blockIndicator->x = blockX/videoW*screenW*scale+x;
	blockIndicator->y = blockY/videoH*screenH*scale+y;
	blockIndicator->height = blockH;
	blockIndicator->scale = scale;
	blockIndicator->alpha = alpha;
	blockIndicator->mirrorX = (blockVictim == 2);
	blockIndicator->start();
	effects.addChild(blockIndicator);
}
void testApp::showBlocking(float blockingX,float blockingY, float blockingW, float blockingH, int blockingVictim)
{
	blockingX = blockingX+blockingW/2;
	blockingY = blockingY+blockingH/2;
	
	BlockingIndicator *blockingIndicator = new BlockingIndicator();
	blockingIndicator->x = blockingX/videoW*screenW*scale+x;
	blockingIndicator->y = blockingY/videoH*screenH*scale+y;
	blockingIndicator->height = blockingH;
	blockingIndicator->scale = scale;
	blockingIndicator->alpha = alpha;
	blockingIndicator->mirrorX = (blockingVictim == 2);
	blockingIndicator->start();
	effects.addChild(blockingIndicator);	
}



//--------------------------------------------------------------
void testApp::update()
{
	//cout << "testApp::update\n";
	boundingBoxes.clear();
	ofxOscMessage m;
	while(receiver.getNextMessage(&m))
	{
		cout << "testApp::update::NextMessage: " << m.getAddress() << ":  ";
		switch (ofToInt(m.getAddress()))
		{
			case HIT_ADDRESS:
				{
				cout << "HIT_ADDRESS\n";
				int hitX	= m.getArgAsInt32(0);
				int hitY	= m.getArgAsInt32(1);
				int hitW	= m.getArgAsInt32(2);
				int hitH	= m.getArgAsInt32(3);
				int hitType = m.getArgAsInt32(4);
				cout << "  hitType: "<<hitType<<"\n";
				addHit(hitX,hitY,hitW,hitH,hitType);
				}
				break;
			case BLOCK:
			{
				cout << "BLOCK\n";
				int blockX = m.getArgAsInt32(0);
				int blockY = m.getArgAsInt32(1);
				int blockW = m.getArgAsInt32(2);
				int blockH = m.getArgAsInt32(3);
				int blockVictim = m.getArgAsInt32(4);
				addBlock(blockX,blockY,blockW,blockH,blockVictim);
			}
				break;
			case BLOCKING:
			{
				cout << "BLOCKING\n";
				int blockX = m.getArgAsInt32(0);
				int blockY = m.getArgAsInt32(1);
				int blockW = m.getArgAsInt32(2);
				int blockH = m.getArgAsInt32(3);
				int blockVictim = m.getArgAsInt32(4);
				showBlocking(blockX,blockY,blockW,blockH,blockVictim);
			}
				break;
			case BOUNDING_BOX:
				if(debug)
				{
					cout << "BOUNDING_BOX\n";
					ofRectangle * boundingBox = new ofRectangle();						
					boundingBox->x = float(m.getArgAsInt32(0))/videoW*screenW*scale+x;
					boundingBox->y = float(m.getArgAsInt32(1))/videoH*screenH*scale+y;
					boundingBox->width = float(m.getArgAsInt32(2))/videoW*screenW*scale;
					boundingBox->height = float(m.getArgAsInt32(3))/videoH*screenH*scale;
					
					boundingBoxes.push_back(boundingBox);
				}
				break;
			case HEALTH:
				cout << "HEALTH\n";
				healthBarPlayer1.percentage = m.getArgAsFloat(0);
				healthBarPlayer2.percentage = m.getArgAsFloat(1);
				break;
			case STATE:
				cout << "  STATE\n";
				setState(m.getArgAsInt32(0));
				break;
			case WINNER:
				cout << "  WINNER\n";
				winner = m.getArgAsInt32(0);
				cout << "  winner: " << winner << "\n";
				break;
			case PLAYERS_UPDATE:
				cout << "  PLAYERS_UPDATE\n";
				detectedPlayer1 = (m.getArgAsInt32(0) == 1)? true : false;
				detectedPlayer2 = (m.getArgAsInt32(1) == 1)? true : false;
				cout << "  detectedPlayer1: " << detectedPlayer1 << "\n";
				cout << "  detectedPlayer2: " << detectedPlayer2 << "\n";
				updatePlayerImages();
				break;
			case COUNT_DOWN:
				cout << "  COUNT_DOWN\n";
				countDown = m.getArgAsInt32(0);
				cout << "  countDown: " << countDown << "\n";
				updateCountDown();
				break;
			case POWERHIT_UPDATE:
				cout << "  POWERHIT_UPDATE\n";
				powerhitPlayer1 = (m.getArgAsInt32(0) == 1)? true : false;
				powerhitPlayer2 = (m.getArgAsInt32(1) == 1)? true : false;
				break;
		}
		m.clear();
	}
	for(int i = 0;i<effects.size();i++)
	{
		Effect* effect = (Effect*)effects.getChildAt(i);
		if(!effect->running)
		{
			effects.removeChildAt(i);
			i--;
		}
	}
}
void testApp::setState(int state)
{
	cout << "testApp::setState: " << state << "\n";
	if(this->state == state || lockState) return;
	
	cout << "  check passed \n";
	
	prevState = this->state;	
	this->state = state;
	
	images.clear();
	disableDraw = false;
	switch(state)
	{
		case STATE_DEMO:
		{
			cout << "  STATE_DEMO\n";
			
			healthBarPlayer1.percentage = 1;
			healthBarPlayer2.percentage = 1;
			
			Image * bgImg = new Image();
			bool loaded = bgImg->img.loadImage("images/demo.png");
			if(!loaded) cout << "Loading demo image failed\n";
			images.addChild(bgImg);
			
			cout << "  creating player1Img image\n";
			player1Img = new Image();
			images.addChild(player1Img);
			
			cout << "  creating player2Img image\n";
			player2Img = new Image();
			player2Img->x = ofGetWidth()/2;
			images.addChild(player2Img);

			updatePlayerImages();
		}
		break;
		case STATE_COUNT_DOWN:
		{
			cout << "STATE_COUNT_DOWN\n";
			
			player1Img = new Image();
			images.addChild(player1Img);
			
			player2Img = new Image();
			player2Img->x = ofGetWidth()/2;
			images.addChild(player2Img);
			
			countDownImg = new Image();
			images.addChild(countDownImg);
			
			updatePlayerImages();
		}
		break;
		case STATE_GAME:
		{
			cout << "STATE_GAME\n";
			Image * bgImg = new Image();
			bgImg->img.loadImage("images/game.png");
			images.addChild(bgImg);
			
			player1Img = new Image();
			images.addChild(player1Img);
			
			player2Img = new Image();
			player2Img->x = ofGetWidth()/2;
			images.addChild(player2Img);
			
			updatePlayerImages();
		}
		break;
		case STATE_WAITING:
		{
			cout << "STATE_WAITING\n";
			
			Image * bgImg = new Image();
			bgImg->img.loadImage("images/game.png");
			images.addChild(bgImg);
			
			player1Img = new Image();
			images.addChild(player1Img);
			
			player2Img = new Image();
			player2Img->x = ofGetWidth()/2;
			images.addChild(player2Img);
			
			updatePlayerImages();
		}
		break;
		case STATE_GAME_FINISHED:
		{
			cout << "STATE_GAME_FINISHED\n";
			cout << "  winner: " << winner << "\n";
			
			Image * img = new Image();
			
			if(winner == 1)
				img->img.loadImage("images/victory1.png");
			else if(winner == 2)
				img->img.loadImage("images/victory2.png");
			
			images.addChild(img);
		}
		break;
		case STATE_GRID:
		{
			cout << "STATE_GRID\n";
			
			Image * img = new Image();
			img->img.loadImage("images/grid.jpg");
			images.addChild(img);
		}
		break;
		case STATE_EMPTY:
		{
			cout << "STATE_EMPTY\n";

			// no children
			
			disableDraw = true;
		}
		break;
		default:
			cout << "DEFAULT (state) \n";
		break;
	}
}
void testApp::updatePlayerImages()
{
	cout << "testApp::updatePlayerImages\n";
	cout << "  detectedPlayer1: "<<detectedPlayer1<<"\n";
	cout << "  state: "<<state<<"\n";
	string player1ImgURL = "";
	if(detectedPlayer1)
	{
		if(state != STATE_GAME && state != STATE_WAITING)
			player1ImgURL = "images/detected1.png";
	}
	else
	{
		if(state == STATE_DEMO)
			player1ImgURL = "images/demo1.png";
		else if(state == STATE_WAITING)
			player1ImgURL = "images/waiting1.png";
	}
	if(player1ImgURL != "")
	{
		cout << "  player1ImgURL: "<<player1ImgURL<<"\n";
		player1Img->img.allocate(512, 768, OF_IMAGE_COLOR_ALPHA);
		bool loaded = player1Img->img.loadImage(player1ImgURL);
		if(!loaded) cout << "Loading player1 image failed (" << player1ImgURL << ")\n";
	}
	else 
	{
		player1Img->img.clear();
	}
	
	string player2ImgURL = "";
	if(detectedPlayer2)
	{
		if(state != STATE_GAME && state != STATE_WAITING)
			player2ImgURL = "images/detected2.png";
	}
	else
	{
		if(state == STATE_DEMO)
			player2ImgURL = "images/demo2.png";
		else if(state == STATE_WAITING)
			player2ImgURL = "images/waiting2.png";
	}
	if(player2ImgURL != "")
	{
		player2Img->img.allocate(512, 768, OF_IMAGE_COLOR_ALPHA);
		bool loaded = player2Img->img.loadImage(player2ImgURL);
		if(!loaded) cout << "Loading player2 image failed (" << player2ImgURL << ")\n";
	}
	else 
	{
		player2Img->img.clear();
	}

	
}
void testApp::updateCountDown()
{
	cout << "testApp::updateCountDown\n";
	cout << "  countDown: " << countDown << "\n";
	string count = ofToString(countDown);
	cout << "  count: " << count << "\n";
	string url = "images/countdown/countdown"+count+".png";
	cout << "  url: " << url << "\n";
	countDownImg->img.loadImage(url);
	
	if(countDown == 0)
		countDownImg->y = -40;
	
	cout << "  countdown image updated\n";
}
//--------------------------------------------------------------
void testApp::draw()
{
	ofBackground(0, 0, 0);
	//ofBackground(125,125,125);
	
	if(disableDraw) return;
	
	//cout << "testApp::draw\n";
	//cout << "  state: " << ofToString(state) << "\n";
	for(int i = 0;i<effects.size();i++)
	{
		HitIndicator* hitIndicator = (HitIndicator*)effects.getChildAt(i);
		hitIndicator->draw();
	}
	
	if(state == STATE_GAME || state == STATE_WAITING)
	{
		int marginX = (screenW-healthBarPlayer1.width-healthBarPlayer2.width)/3;
		int marginY = 20;
		ofEnableAlphaBlending();
		if(powerhitPlayer1)
			powerhit1Img.draw(marginX+healthBarPlayer1.width-powerhit1Img.width,marginY+healthBarPlayer1.height);
		if(powerhitPlayer2)
			powerhit2Img.draw(healthBarPlayer2.x,marginY+healthBarPlayer2.height);
		ofDisableAlphaBlending();
	}
	if(state == STATE_GAME || state == STATE_GAME_FINISHED || state == STATE_WAITING)
	{
		healthBarPlayer1.draw();
		healthBarPlayer2.draw();
	}
	
	if(state != STATE_GAME && state != STATE_GAME_FINISHED)
	{
		ofDisableAlphaBlending();
		ofSetColor(255,255,255,255);
	}
	
	if(debug && state == STATE_GAME)
	{
		ofEnableAlphaBlending();
		ofSetColor(255,255,255,50);
		//ofSetColor(255,255,255);
		
		//cout << "  boundingBoxes.size(): " << boundingBoxes.size() << "\n";
		
		for(int i = 0;i<boundingBoxes.size();i++)
		{
			ofRectangle* boundingBox = boundingBoxes.at(i);
			ofNoFill();
			ofSetLineWidth(5);
			ofRect(boundingBox->x,boundingBox->y,boundingBox->width,boundingBox->height);
		}
		
		/*ofNoFill();
		ofSetLineWidth(20);
		ofRect(0,0,1024,768);
		
		ofFill();
		ofEllipse(1024/2,768/2,50,50);*/
		ofSetColor(255,255,255,255);
		ofDisableAlphaBlending();
	}
}

//--------------------------------------------------------------
void testApp::keyPressed(int keyCode)
{
}

//--------------------------------------------------------------
void testApp::keyReleased(int keyCode)
{
	float step;
	if(ofKeyShift()) step = 10;
	else if(ofKeyControl()) step = 1;
	else step = 5;
	
	switch (keyCode){
		case ' ':
			ofToggleFullscreen();
			break;
		case '+':
		case '=':
			scale += step/100;
			break;
		case '-':
		case '_':
			scale -= step/100;
			break;
		case OF_KEY_UP:
			y -= step;
			break;
		case OF_KEY_DOWN:
			y += step;
			break;
		case OF_KEY_LEFT:
			x -= step;
			break;
		case OF_KEY_RIGHT:
			x += step;
			break;
		case ',':
			alpha -= step;
			break;
		case '.':
			alpha += step;
			break;
		case 'd':
			debug = !debug;
			break;
		case 'h':
			if(state == STATE_EMPTY)
			{
				lockState = false;
				setState(prevState);
			}
			else
			{
				setState(STATE_EMPTY);
				lockState = true;
			}
			break;
		case 'g':
			if(state == STATE_GRID)
			{
				lockState = false;
				setState(prevState);
			}
			else
			{
				setState(STATE_GRID);
				lockState = true;
			}
			break;
		case 'q':
			healthBarPlayer1.percentage -= 0.1;
			if(healthBarPlayer1.percentage < 0)
				healthBarPlayer1.percentage = 0;
			healthBarPlayer2.percentage -= 0.1;
			if(healthBarPlayer2.percentage < 0)
				healthBarPlayer2.percentage = 0;
			break;
		case 'w':
			healthBarPlayer1.percentage += 0.01;
			healthBarPlayer2.percentage += 0.01;
			break;
		case OF_KEY_RETURN:
			setState(STATE_GAME);
			break;
	}
	cout << "x: " << x << " y: " << y << " scale: " << scale << "\n";
	storeValues();
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y )
{

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{
	addHit(x,y,10,10,HIT_LEGS);
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{
	//ofToggleFullscreen();
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{

}


