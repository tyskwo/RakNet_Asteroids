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

	while (!mpClient->getConnected()) { mpClient->update(); }

	while (window.isOpen())
	{
		mpClient->update();

		interpolateOtherPlayer();

		mpClient->mpGame->update();
		getInfoFromGame();

		getInput();
		
		drawScreen(window);

		sf::Event event;
		while (window.pollEvent(event)) { if (event.type == sf::Event::Closed) window.close(); }
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) { window.close(); }
	}

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
		const char* serverIP = argv[1];
		const char* serverPort = argv[2];
		const char* clientPort = argv[3];

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

	pWindow.draw(*mpClient->mpGame->getFirstPlayer()->getSprite());
	pWindow.draw(*mpClient->mpGame->getSecondPlayer()->getSprite());

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
			mpClient->mpGame->spawnAsteroid();
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

	/*
	for (unsigned int i = 0; i < mpGame->getAsteroids().size(); i++)
	{
		Asteroid* pTempAsteroid = mpGame->getAsteroids()[i];

		if (pTempAsteroid != NULL)
		{
			info.asteroids[i].health = pTempAsteroid->getHealth();
			info.asteroids[i].position.x = pTempAsteroid->getBody()->GetPosition().x;
			info.asteroids[i].position.y = pTempAsteroid->getBody()->GetPosition().y;
			info.asteroids[i].size = pTempAsteroid->getSize();
			info.asteroids[i].velocity.x = pTempAsteroid->getBody()->GetLinearVelocity().x;
			info.asteroids[i].velocity.y = pTempAsteroid->getBody()->GetLinearVelocity().y;
			info.asteroids[i].velocity.rot = pTempAsteroid->getBody()->GetAngle();
		}
	}
	
	for (unsigned int i = 0; i < mpGame->getFirstPlayerBullets().size(); i++)
	{
		Bullet* pBullet = mpGame->getFirstPlayerBullets()[i];

		if (pBullet != NULL)
		{
			info.firstPlayer.bullets[i].position.x = pBullet->getBody()->GetPosition().x;
			info.firstPlayer.bullets[i].position.y = pBullet->getBody()->GetPosition().y;
			info.firstPlayer.bullets[i].velocity.x = pBullet->getBody()->GetLinearVelocity().x;
			info.firstPlayer.bullets[i].velocity.y = pBullet->getBody()->GetLinearVelocity().y;
			info.firstPlayer.bullets[i].velocity.rot = pBullet->getBody()->GetAngle();
		}
	}

	for (unsigned int i = 0; i < mpGame->getSecondPlayerBullets().size(); i++)
	{
		Bullet* pBullet = mpGame->getSecondPlayerBullets()[i];

		if (pBullet != NULL)
		{
			info.secondPlayer.bullets[i].position.x = pBullet->getBody()->GetPosition().x;
			info.secondPlayer.bullets[i].position.y = pBullet->getBody()->GetPosition().y;
			info.secondPlayer.bullets[i].velocity.x = pBullet->getBody()->GetLinearVelocity().x;
			info.secondPlayer.bullets[i].velocity.y = pBullet->getBody()->GetLinearVelocity().y;
			info.secondPlayer.bullets[i].velocity.rot = pBullet->getBody()->GetAngle();
		}
	}*/

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
	if (mpClient->hasStates())
	{
		if (mpClient->getFirstConnected())
		{
			if (mpClient->mpGame->getSecondPlayer()->isFinishedInterpolating())
			{
				BothShips bestState = mpClient->getBestState();
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
				BothShips bestState = mpClient->getBestState();
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