//SFML includes
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

//Box2D includes
#include <Box2D\Box2D.h>

//library includes
#include <iostream>
#include <sstream>
#include <string>

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

Game* mpGame;

sf::Texture TEXTURES::mFirstShip,       TEXTURES::mSecondShip,
			TEXTURES::mFirstBullet,     TEXTURES::mSecondBullet,
			TEXTURES::mSmallAsteroid1,  TEXTURES::mSmallAsteroid2,  TEXTURES::mSmallAsteroid3,
			TEXTURES::mMediumAsteroid1, TEXTURES::mMediumAsteroid2, TEXTURES::mMediumAsteroid3,
			TEXTURES::mLargeAsteroid1,  TEXTURES::mLargeAsteroid2,  TEXTURES::mLargeAsteroid3;

























//WHERE IN THE WORLD SHOULD I PUT THIS THING!!!!!!!!!
bool isSpacePressed = false;
bool isPPressed = false;





















//###METHOD DECLARATIONS###############################################################################################

	Client* initClient(int argc, char** argv);		//init client

	void initFont();
	void initWindow(sf::RenderWindow &pWindow);		//init font and window

	void drawScreen(sf::RenderWindow &pWindow);		//draw to window
	void getInput();								//get keyboard input

	char* getPortNumber();							//generate random port number




int main(int argc, char** argv)
{
	srand(static_cast<unsigned int>(time(NULL)));

//###DETERMINE ENVIRONMENT#############################################################################################
	#ifdef _DEBUG
		mBuildType = "Debug/";
	#else
		mBuildType = "Release/";
	#endif




//###INIT WINDOW#######################################################################################################
	sf::RenderWindow window(sf::VideoMode(static_cast<unsigned int>(SCREEN_WIDTH), 
		                                  static_cast<unsigned int>(SCREEN_HEIGHT)), "Asteroids");
	initWindow(window);




//###INIT CLIENT#######################################################################################################
	mpClient = initClient(argc, argv);



//###INIT TEXTURES#####################################################################################################
	TEXTURES::init(mBuildType);


	

//###GAME INIT#########################################################################################################
	mpGame = new Game();




//###WAIT UNTIL CONNECTED TO RUN GAME LOOP#############################################################################
	while (!mpClient->getConnected()) { mpClient->update(); }




//###RUN PROGRAM######################################################################################################
	while (window.isOpen())
	{
//###UPDATE CLIENT####################################################################################################
		mpClient->update();

//###UPDATE GAME######################################################################################################
		mpGame->update();

//###GET INPUT########################################################################################################
		getInput();
		
//###DRAW#############################################################################################################
		drawScreen(window);

//###CHECK TO CLOSE WINDOW############################################################################################
		sf::Event event;
		while (window.pollEvent(event))
		{
			//close the window
			if (event.type == sf::Event::Closed)
				window.close();
		}
	}

	return 0;
};








//###METHOD IMPLEMENTATIONS###########################################################################################

Client* initClient(int argc, char** argv)
{
	//command line arguments
	if (argc == 4) //serverIP serverPort clientPort
	{
		mBuildType = "";

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

//init window, etc.
void initFont() { mFont.loadFromFile(mBuildType + "font.otf"); }
void initWindow(sf::RenderWindow &pWindow)
{
	initFont();

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
	//clear the display
	pWindow.clear(sf::Color(37, 37, 37));

	pWindow.draw(*mpGame->getFirstPlayer()->getSprite());
	pWindow.draw(*mpGame->getSecondPlayer()->getSprite());

	for (unsigned int i = 0; i < mpGame->getFirstPlayerBullets().size(); i++)
	{
		if (mpGame->getFirstPlayerBullets()[i] != NULL)
		{
 			pWindow.draw(*mpGame->getFirstPlayerBullets()[i]->getSprite());
		}
	}

	for (unsigned int i = 0; i < mpGame->getSecondPlayerBullets().size(); i++)
	{
		if (mpGame->getSecondPlayerBullets()[i] != NULL)
		{
			pWindow.draw(*mpGame->getSecondPlayerBullets()[i]->getSprite());
		}
	}

	for (unsigned int i = 0; i < mpGame->getAsteroids().size(); i++)
	{
		if (mpGame->getAsteroids()[i] != NULL)
		{
			pWindow.draw(*mpGame->getAsteroids()[i]->getSprite());
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
			mpGame->getFirstPlayer()->getBody()->SetLinearVelocity(b2Vec2(mpGame->getFirstPlayer()->getBody()->GetLinearVelocity().x + mpGame->getFirstPlayer()->getLinearSpeed()*cos(mpGame->getFirstPlayer()->getBody()->GetAngle()),
				mpGame->getFirstPlayer()->getBody()->GetLinearVelocity().y + mpGame->getFirstPlayer()->getLinearSpeed()*sin(mpGame->getFirstPlayer()->getBody()->GetAngle())));
		}

		//left arrow key or A is pressed down
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		{
			mpGame->getFirstPlayer()->getBody()->SetAngularVelocity(mpGame->getFirstPlayer()->getBody()->GetAngularVelocity() - mpGame->getFirstPlayer()->getAngularSpeed());
			mpGame->getFirstPlayer()->getSprite()->setRotation(mpGame->getFirstPlayer()->getBody()->GetAngle() * 180.0f / 3.14159f);
		}

		//right arrow key or D is pressed down
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{
			mpGame->getFirstPlayer()->getBody()->SetAngularVelocity(mpGame->getFirstPlayer()->getBody()->GetAngularVelocity() + mpGame->getFirstPlayer()->getAngularSpeed());
			mpGame->getFirstPlayer()->getSprite()->setRotation(mpGame->getFirstPlayer()->getBody()->GetAngle() * 180.0f / 3.14159f);
		}

		//space key is pressed down
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			if (!isSpacePressed)
			{
				mpGame->fireBullet(true);
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
			mpGame->getSecondPlayer()->getBody()->SetLinearVelocity(b2Vec2(mpGame->getSecondPlayer()->getBody()->GetLinearVelocity().x + mpGame->getSecondPlayer()->getLinearSpeed()*cos(mpGame->getSecondPlayer()->getBody()->GetAngle()),
				mpGame->getSecondPlayer()->getBody()->GetLinearVelocity().y + mpGame->getSecondPlayer()->getLinearSpeed()*sin(mpGame->getSecondPlayer()->getBody()->GetAngle())));
		}

		//left arrow key or A is pressed down
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		{
			mpGame->getSecondPlayer()->getBody()->SetAngularVelocity(mpGame->getSecondPlayer()->getBody()->GetAngularVelocity() - mpGame->getSecondPlayer()->getAngularSpeed());
			mpGame->getSecondPlayer()->getSprite()->setRotation(mpGame->getSecondPlayer()->getBody()->GetAngle() * 180.0f / 3.14159f);
		}

		//right arrow key or D is pressed down
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{
			mpGame->getSecondPlayer()->getBody()->SetAngularVelocity(mpGame->getSecondPlayer()->getBody()->GetAngularVelocity() + mpGame->getSecondPlayer()->getAngularSpeed());
			mpGame->getSecondPlayer()->getSprite()->setRotation(mpGame->getSecondPlayer()->getBody()->GetAngle() * 180.0f / 3.14159f);
		}

		//space key is pressed down
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			if (!isSpacePressed)
			{
				mpGame->fireBullet(false);
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
			mpGame->spawnAsteroid();
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
	GameInfo info;

	for (unsigned int i = 0; i < mpGame->getAsteroids().size(); i++)
	{
		Asteroid* pTempAsteroid = mpGame->getAsteroids()[i];
		info.asteroids[i].health = pTempAsteroid->getHealth();
		info.asteroids[i].position.x = pTempAsteroid->getBody()->GetPosition().x;
		info.asteroids[i].position.y = pTempAsteroid->getBody()->GetPosition().y;
		info.asteroids[i].size = pTempAsteroid->getSize();
		info.asteroids[i].velocity.x = pTempAsteroid->getBody()->GetLinearVelocity().x;
		info.asteroids[i].velocity.y = pTempAsteroid->getBody()->GetLinearVelocity().y;
		info.asteroids[i].velocity.rot = pTempAsteroid->getBody()->GetAngle();
	}
	
	for (unsigned int i = 0; i < mpGame->getFirstPlayerBullets().size(); i++)
	{
		info.firstPlayer.bullets[i].position.x = mpGame->getFirstPlayerBullets()[i]->getBody()->GetPosition().x;
		info.firstPlayer.bullets[i].position.y = mpGame->getFirstPlayerBullets()[i]->getBody()->GetPosition().y;
		info.firstPlayer.bullets[i].velocity.x = mpGame->getFirstPlayerBullets()[i]->getBody()->GetLinearVelocity().x;
		info.firstPlayer.bullets[i].velocity.y = mpGame->getFirstPlayerBullets()[i]->getBody()->GetLinearVelocity().y;
		info.firstPlayer.bullets[i].velocity.rot = mpGame->getFirstPlayerBullets()[i]->getBody()->GetAngle();
	}

	for (unsigned int i = 0; i < mpGame->getSecondPlayerBullets().size(); i++)
	{
		info.secondPlayer.bullets[i].position.x = mpGame->getSecondPlayerBullets()[i]->getBody()->GetPosition().x;
		info.secondPlayer.bullets[i].position.y = mpGame->getSecondPlayerBullets()[i]->getBody()->GetPosition().y;
		info.secondPlayer.bullets[i].velocity.x = mpGame->getSecondPlayerBullets()[i]->getBody()->GetLinearVelocity().x;
		info.secondPlayer.bullets[i].velocity.y = mpGame->getSecondPlayerBullets()[i]->getBody()->GetLinearVelocity().y;
		info.secondPlayer.bullets[i].velocity.rot = mpGame->getSecondPlayerBullets()[i]->getBody()->GetAngle();
	}

	info.firstPlayer.health = mpGame->getFirstPlayer()->getHealth();
	info.firstPlayer.position.x = mpGame->getFirstPlayer()->getBody()->GetPosition().x;
	info.firstPlayer.position.y = mpGame->getFirstPlayer()->getBody()->GetPosition().y;
	info.firstPlayer.velocity.x = mpGame->getFirstPlayer()->getBody()->GetLinearVelocity().x;
	info.firstPlayer.velocity.y = mpGame->getFirstPlayer()->getBody()->GetLinearVelocity().y;
	info.firstPlayer.velocity.rot = mpGame->getFirstPlayer()->getBody()->GetAngle();

	info.secondPlayer.health = mpGame->getSecondPlayer()->getHealth();
	info.secondPlayer.position.x = mpGame->getSecondPlayer()->getBody()->GetPosition().x;
	info.secondPlayer.position.y = mpGame->getSecondPlayer()->getBody()->GetPosition().y;
	info.secondPlayer.velocity.x = mpGame->getSecondPlayer()->getBody()->GetLinearVelocity().x;
	info.secondPlayer.velocity.y = mpGame->getSecondPlayer()->getBody()->GetLinearVelocity().y;
	info.secondPlayer.velocity.rot = mpGame->getSecondPlayer()->getBody()->GetAngle();
}