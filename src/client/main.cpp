//SFML includes
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

//Box2D includes
#include <Box2D\Box2D.h>

//library includes
#include <iostream>
#include <sstream>
#include <string>
#include "..\common\MemoryTracker.h"

//game includes
#include "Client.h"
#include "..\common\Game.h"
#include "..\common\PhysicsStructs.h"
#include "..\common\Ship.h"
#include "..\common\Bullet.h"
#include "..\common\Asteroid.h"


std::string mBuildType = "";	//to determine file paths of assets

Client* mpClient;
sf::Font mFont;

sf::Text HUDtext;
sf::Text p1Health;
sf::Text p2Health;
std::stringstream p1healthStream;
std::stringstream p2healthStream;
std::stringstream roundStream;

std::array<sf::Sprite, 8> markers;
std::array<sf::Sprite, 20> p1Lives, p2Lives;
sf::Texture TEXTURES::mFirstShip,       TEXTURES::mSecondShip,
			TEXTURES::mFirstBullet,     TEXTURES::mSecondBullet,
			TEXTURES::mSmallAsteroid1,  TEXTURES::mSmallAsteroid2,  TEXTURES::mSmallAsteroid3,
			TEXTURES::mMediumAsteroid1, TEXTURES::mMediumAsteroid2, TEXTURES::mMediumAsteroid3,
			TEXTURES::mLargeAsteroid1,  TEXTURES::mLargeAsteroid2,  TEXTURES::mLargeAsteroid3,
			TEXTURES::mMarker;

//input flags
bool isSpacePressed = false;
bool isPPressed = false;



//###METHOD DECLARATIONS###############################################################################################

	Client* initClient(int argc, char** argv);		//init client

	void initWindow(sf::RenderWindow &pWindow);		//init window

	void drawScreen(sf::RenderWindow &pWindow);		//draw to window
	void getInput();								//get keyboard input
	void getInfoFromGame();

	void interpolateOtherPlayer();
	void interpolateAsteroids();

	char* getPortNumber();							//generate random port number




int main(int argc, char** argv)
{
	srand(static_cast<unsigned int>(time(NULL)));

	#ifdef _DEBUG
		mBuildType = "Debug/";
	#else
		mBuildType = "Release/";
	#endif
		if (argc == 4) mBuildType = "";

	sf::RenderWindow window(sf::VideoMode(static_cast<unsigned int>(SCREEN_WIDTH), 
		                                  static_cast<unsigned int>(SCREEN_HEIGHT)), "Asteroids");
	initWindow(window);

	mpClient = initClient(argc, argv);
	TEXTURES::init(mBuildType);
	mpClient->initGame();

	HUDtext = sf::Text("ROUND 1\nSCORE 0", mFont, 60);
	
	for (unsigned int i = 0; i < markers.size(); i++)
	{
		markers[i] = sf::Sprite(TEXTURES::mMarker);
	}

	for (unsigned int i = 0; i < p1Lives.size(); i++)
	{
		p1Lives[i] = sf::Sprite(TEXTURES::mFirstShip);
		p2Lives[i] = sf::Sprite(TEXTURES::mSecondShip);
		p1Lives[i].setPosition(17.5f + 10.0f * i, SCREEN_HEIGHT - 100.0f);
		p2Lives[i].setPosition(SCREEN_WIDTH - 250.0f + 10.0f * i, SCREEN_HEIGHT - 100.0f);
	}

	while (!mpClient->getConnected()) { mpClient->update(); }

	while (window.isOpen())
	{
		mpClient->update();

		interpolateOtherPlayer();
		interpolateAsteroids();

		mpClient->mpGame->update();
		getInfoFromGame();

		getInput();
		
		drawScreen(window);

		sf::Event event;
		while (window.pollEvent(event)) { if (event.type == sf::Event::Closed) window.close(); }
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) { window.close(); }
	}

	delete mpClient;
	mpClient = NULL;

	gMemoryTracker.reportAllocations(std::cout);
	system("pause");

	return 0;
};


//###METHOD IMPLEMENTATIONS###########################################################################################

Client* initClient(int argc, char** argv)
{
	//command line arguments
	if (argc == 4) //serverIP serverPort clientPort
	{
		const char* serverIP = argv[2];
		const char* serverPort = argv[3];
		const char* clientPort = argv[1];

		return new Client(clientPort, serverIP, serverPort);
	}
	else //manually get parameters
	{
		//connect to a random port
		char* clientPort = getPortNumber();

		//connect to external ip
		std::string serverIP = "";
		std::cout << "Input server IP or \"localhost\": ";
		std::cin >> serverIP;
		return new Client(clientPort, serverIP.c_str(), "200");
	}
}

void initWindow(sf::RenderWindow &pWindow)
{
	mFont.loadFromFile(mBuildType + "font.otf");
	pWindow.setFramerateLimit(60);
	pWindow.clear(sf::Color(37, 37, 37));

	sf::Text beginText;
	beginText.setString("CONNECTING...");
	beginText.setCharacterSize(95);
	beginText.setFont(mFont);
	beginText.setColor(sf::Color(255, 255, 255, 100));
	beginText.setOrigin(beginText.getLocalBounds().width / 2.0f, beginText.getLocalBounds().height / 2.0f);
	beginText.setPosition(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f);

	pWindow.draw(beginText);
	pWindow.display();
}

//draw the client's game info
void drawScreen(sf::RenderWindow &pWindow)
{
	pWindow.clear(sf::Color(37, 37, 37));
	pWindow.draw(HUDtext);

	roundStream << mpClient->mpGame->getRoundNum();

	HUDtext.setString("Round: " + roundStream.str());

	pWindow.draw(*mpClient->mpGame->getFirstPlayer()->getSprite());
	pWindow.draw(*mpClient->mpGame->getSecondPlayer()->getSprite());

	
	p1healthStream << mpClient->mpGame->getFirstPlayer()->getHealth();
	p2healthStream << mpClient->mpGame->getSecondPlayer()->getHealth();

	p1Health.setString(p1healthStream.str());
	p1Health.setCharacterSize(60);
	p1Health.setFont(mFont);
	p1Health.setColor(sf::Color(255, 255, 255, 100));
	p1Health.setOrigin(0.0f, p1Health.getLocalBounds().height);
	p1Health.setPosition(10.0f, SCREEN_HEIGHT - 10.0f);

	p2Health.setString(p2healthStream.str());
	p2Health.setCharacterSize(60);
	p2Health.setFont(mFont);
	p2Health.setColor(sf::Color(255, 255, 255, 100));
	p2Health.setOrigin(p2Health.getLocalBounds().height, p2Health.getLocalBounds().height);
	p2Health.setPosition(SCREEN_WIDTH - 10.0f, SCREEN_HEIGHT - 10.0f);

	roundStream.str("");
	p1healthStream.clear();
	p2healthStream.clear();
	roundStream.clear();

	pWindow.draw(p1Health);
	pWindow.draw(p2Health);



	for (unsigned int i = 0; i < mpClient->mpGame->getFirstPlayerBullets().size(); i++)
	{
		if (mpClient->mpGame->getFirstPlayerBullets()[i] != NULL)
		{
			pWindow.draw(*mpClient->mpGame->getFirstPlayerBullets()[i]->getSprite());
		}
		if (mpClient->mpGame->getSecondPlayerBullets()[i] != NULL)
		{
			pWindow.draw(*mpClient->mpGame->getSecondPlayerBullets()[i]->getSprite());
		}
	}

	for (unsigned int i = 0; i < mpClient->mpGame->getAsteroids().size(); i++)
	{
		if (mpClient->mpGame->getAsteroids()[i] != NULL)
		{
			if (mpClient->mpGame->getAsteroids()[i]->isOnScreen())
			{
				pWindow.draw(*mpClient->mpGame->getAsteroids()[i]->getSprite());
			}
			else
			{
				//draw marker at edge of screen.
				if (i < markers.size())
				{
					markers[i].setOrigin(markers[i].getLocalBounds().width / 2.0f, markers[i].getLocalBounds().height / 2.0f);
					b2Vec2 position(mpClient->mpGame->getAsteroids()[i]->getBody()->GetPosition());

					sf::Vector2f markerPosition;
					markerPosition.x = position.x;
					markerPosition.y = position.y;

					float markerWidth = markers[i].getLocalBounds().width / 2.0f + 10.0f;
					float markerHeight = markers[i].getLocalBounds().height / 2.0f + 10.0f;

					if (position.x < 0)             { markerPosition.x = markerWidth; }
					else if (position.x > SCREEN_WIDTH)  { markerPosition.x = SCREEN_WIDTH - markerWidth; }
					if (position.y < 0)             { markerPosition.y = markerHeight; }
					else if (position.y > SCREEN_HEIGHT) { markerPosition.y = SCREEN_HEIGHT - markerHeight; }

					markers[i].setPosition(markerPosition);

					pWindow.draw(markers[i]);
				}
			}
		}
	}

	//update the display
	pWindow.display();
};

//get keyboard/mouse input
void getInput()
{
	if (mpClient->getFirstConnected())
	{
		//up arrow key or W is pressed down
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		{
			mpClient->mpGame->getFirstPlayer()->getBody()->SetLinearVelocity(b2Vec2(mpClient->mpGame->getFirstPlayer()->getBody()->GetLinearVelocity().x + mpClient->mpGame->getFirstPlayer()->getLinearSpeed()*cos(mpClient->mpGame->getFirstPlayer()->getBody()->GetAngle()),
				mpClient->mpGame->getFirstPlayer()->getBody()->GetLinearVelocity().y + mpClient->mpGame->getFirstPlayer()->getLinearSpeed()*sin(mpClient->mpGame->getFirstPlayer()->getBody()->GetAngle())));
		}

		//left arrow key or A is pressed down
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		{
			mpClient->mpGame->getFirstPlayer()->getBody()->SetAngularVelocity(mpClient->mpGame->getFirstPlayer()->getBody()->GetAngularVelocity() - mpClient->mpGame->getFirstPlayer()->getAngularSpeed());
			mpClient->mpGame->getFirstPlayer()->getSprite()->setRotation(mpClient->mpGame->getFirstPlayer()->getBody()->GetAngle() * 180.0f / 3.14159f);
		}

		//right arrow key or D is pressed down
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{
			mpClient->mpGame->getFirstPlayer()->getBody()->SetAngularVelocity(mpClient->mpGame->getFirstPlayer()->getBody()->GetAngularVelocity() + mpClient->mpGame->getFirstPlayer()->getAngularSpeed());
			mpClient->mpGame->getFirstPlayer()->getSprite()->setRotation(mpClient->mpGame->getFirstPlayer()->getBody()->GetAngle() * 180.0f / 3.14159f);
		}

		//space key is pressed down
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			if (!isSpacePressed)
			{
				int index = mpClient->mpGame->fireBullet(true);

				if (index != -1)
				{
					BulletData bulletData;
					bulletData.bullet.position.x = mpClient->mpGame->getFirstPlayerBullets()[index]->getBody()->GetPosition().x;
					bulletData.bullet.position.y = mpClient->mpGame->getFirstPlayerBullets()[index]->getBody()->GetPosition().y;
					bulletData.bullet.position.angle = mpClient->mpGame->getFirstPlayerBullets()[index]->getBody()->GetAngle();

					bulletData.bullet.velocity.x = mpClient->mpGame->getFirstPlayerBullets()[index]->getBody()->GetLinearVelocity().x;
					bulletData.bullet.velocity.y = mpClient->mpGame->getFirstPlayerBullets()[index]->getBody()->GetLinearVelocity().y;
					bulletData.bullet.velocity.rot = mpClient->mpGame->getFirstPlayerBullets()[index]->getBody()->GetAngularVelocity();

					bulletData.mID = ID_SEND_BULLET_INFO;
					bulletData.index = index;
					bulletData.timeStamp = RakNet::GetTime();

					mpClient->addBulletData(bulletData);
				}				
				isSpacePressed = true;
			}
		}
		else
		{
			isSpacePressed = false;
		}
	}
	else
	{
		//up arrow key or W is pressed down
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		{
			mpClient->mpGame->getSecondPlayer()->getBody()->SetLinearVelocity(b2Vec2(mpClient->mpGame->getSecondPlayer()->getBody()->GetLinearVelocity().x + mpClient->mpGame->getSecondPlayer()->getLinearSpeed()*cos(mpClient->mpGame->getSecondPlayer()->getBody()->GetAngle()),
				mpClient->mpGame->getSecondPlayer()->getBody()->GetLinearVelocity().y + mpClient->mpGame->getSecondPlayer()->getLinearSpeed()*sin(mpClient->mpGame->getSecondPlayer()->getBody()->GetAngle())));
		}

		//left arrow key or A is pressed down
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		{
			mpClient->mpGame->getSecondPlayer()->getBody()->SetAngularVelocity(mpClient->mpGame->getSecondPlayer()->getBody()->GetAngularVelocity() - mpClient->mpGame->getSecondPlayer()->getAngularSpeed());
			mpClient->mpGame->getSecondPlayer()->getSprite()->setRotation(mpClient->mpGame->getSecondPlayer()->getBody()->GetAngle() * 180.0f / 3.14159f);
		}

		//right arrow key or D is pressed down
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{
			mpClient->mpGame->getSecondPlayer()->getBody()->SetAngularVelocity(mpClient->mpGame->getSecondPlayer()->getBody()->GetAngularVelocity() + mpClient->mpGame->getSecondPlayer()->getAngularSpeed());
			mpClient->mpGame->getSecondPlayer()->getSprite()->setRotation(mpClient->mpGame->getSecondPlayer()->getBody()->GetAngle() * 180.0f / 3.14159f);
		}

		//space key is pressed down
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			if (!isSpacePressed)
			{
				int index = mpClient->mpGame->fireBullet(false);

				if (index != -1)
				{
					BulletData bulletData;
					bulletData.bullet.position.x = mpClient->mpGame->getSecondPlayerBullets()[index]->getBody()->GetPosition().x;
					bulletData.bullet.position.y = mpClient->mpGame->getSecondPlayerBullets()[index]->getBody()->GetPosition().y;
					bulletData.bullet.position.angle = mpClient->mpGame->getSecondPlayerBullets()[index]->getBody()->GetAngle();

					bulletData.bullet.velocity.x = mpClient->mpGame->getSecondPlayerBullets()[index]->getBody()->GetLinearVelocity().x;
					bulletData.bullet.velocity.y = mpClient->mpGame->getSecondPlayerBullets()[index]->getBody()->GetLinearVelocity().y;
					bulletData.bullet.velocity.rot = mpClient->mpGame->getSecondPlayerBullets()[index]->getBody()->GetAngularVelocity();

					bulletData.mID = ID_SEND_BULLET_INFO;
					bulletData.index = index;
					bulletData.timeStamp = RakNet::GetTime();
					
					mpClient->addBulletData(bulletData);
				}
				isSpacePressed = true;
			}
		}
		else
		{
			isSpacePressed = false;
		}
	}

	//p key is pressed down
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
	{
		if (!isPPressed)
		{
			mpClient->sendCheatCode();
			isPPressed = true;
		}
	}
	else
	{
		isPPressed = false;
	}
};

//get random port number from 201 - 401
char* getPortNumber()
{
	//generate random number between 201-401
	srand(static_cast<unsigned int>(time(NULL)));
	int randPortNumber = rand() % (401 - 201 + 1) + 201;

	//convert randPortNumber from int to string
	std::stringstream converter;
	converter << randPortNumber;
	std::string temp_str = converter.str();

	//return char pointer of converted int
	return (char*)temp_str.c_str();
};

void getInfoFromGame()
{
	//GameInfo info;
	BothShips shipData;

	shipData.firstPlayer.health = mpClient->mpGame->getFirstPlayer()->getHealth();
	shipData.firstPlayer.position.x = mpClient->mpGame->getFirstPlayer()->getBody()->GetPosition().x;
	shipData.firstPlayer.position.y = mpClient->mpGame->getFirstPlayer()->getBody()->GetPosition().y;
	shipData.firstPlayer.velocity.x = mpClient->mpGame->getFirstPlayer()->getBody()->GetLinearVelocity().x;
	shipData.firstPlayer.velocity.y = mpClient->mpGame->getFirstPlayer()->getBody()->GetLinearVelocity().y;
	shipData.firstPlayer.velocity.rot = mpClient->mpGame->getFirstPlayer()->getBody()->GetAngularVelocity();
	shipData.firstPlayer.position.angle = mpClient->mpGame->getFirstPlayer()->getBody()->GetAngle();

	shipData.secondPlayer.health = mpClient->mpGame->getSecondPlayer()->getHealth();
	shipData.secondPlayer.position.x = mpClient->mpGame->getSecondPlayer()->getBody()->GetPosition().x;
	shipData.secondPlayer.position.y = mpClient->mpGame->getSecondPlayer()->getBody()->GetPosition().y;
	shipData.secondPlayer.velocity.x = mpClient->mpGame->getSecondPlayer()->getBody()->GetLinearVelocity().x;
	shipData.secondPlayer.velocity.y = mpClient->mpGame->getSecondPlayer()->getBody()->GetLinearVelocity().y;
	shipData.secondPlayer.velocity.rot = mpClient->mpGame->getSecondPlayer()->getBody()->GetAngularVelocity();
	shipData.secondPlayer.position.angle = mpClient->mpGame->getSecondPlayer()->getBody()->GetAngle();

	shipData.timeStamp = RakNet::GetTime();

	mpClient->setShipData(shipData);
}

void interpolateOtherPlayer()
{
	if (mpClient->hasShipStates())
	{
		if (mpClient->getFirstConnected())
		{
			if (mpClient->mpGame->getSecondPlayer()->isFinishedInterpolating())
			{
				BothShips bestState = mpClient->getBestShipState();
				mpClient->mpGame->getSecondPlayer()->addInterpolation(bestState.timeStamp, bestState.secondPlayer);
				mpClient->mpGame->getSecondPlayer()->setDoneInterpolated(false);
			}
			else
			{
				mpClient->mpGame->getSecondPlayer()->interpolate(RakNet::GetTime(), mpClient->mOnePacketAgo, mpClient->mTwoPacketsAgo);
			}
		}
		else
		{
			if (mpClient->mpGame->getFirstPlayer()->isFinishedInterpolating())
			{
				BothShips bestState = mpClient->getBestShipState();
				mpClient->mpGame->getFirstPlayer()->addInterpolation(bestState.timeStamp, bestState.firstPlayer);
				mpClient->mpGame->getFirstPlayer()->setDoneInterpolated(false);
			}
			else
			{
				mpClient->mpGame->getFirstPlayer()->interpolate(RakNet::GetTime(), mpClient->mOnePacketAgo, mpClient->mTwoPacketsAgo);
			}
		}
	}
}

void interpolateAsteroids()
{
	for (unsigned int i = 0; i < mpClient->mAsteroidStates.size(); i++)
	{
		if (mpClient->hasAsteroidStates(i) && mpClient->mpGame->getAsteroid(i) != NULL)
		{
			if (mpClient->mpGame->getAsteroid(i)->isFinishedInterpolating())
			{
				AsteroidObject bestState = mpClient->getBestAsteroidState(i);
				mpClient->mpGame->getAsteroid(i)->addInterpolation(bestState);
				mpClient->mpGame->getAsteroid(i)->setDoneInterpolated(false);
			}
			else
			{
				mpClient->mpGame->getAsteroid(i)->interpolate(RakNet::GetTime(), mpClient->mOnePacketAgo, mpClient->mTwoPacketsAgo);
			}
		}
	}
}