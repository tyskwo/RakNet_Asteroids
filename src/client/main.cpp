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
//#include "../common/PhysicsStructs.h"
#include "../common/Ship.h"
#include "../common/Bullet.h"
#include "../common/Asteroid.h"



std::string mBuildType = "";	//to determine file paths of assets

Client* mpClient;

sf::Font mFont;

b2World* physicsWorld = new b2World(PHYSICS::WORLD::gravity());

Ship* player;
std::array<Bullet*,   16> playerBullets;
std::array<Asteroid*, 64> asteroids;

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

	void initPhysics();								//init physics world
	void updatePhysics();							//update physics world

	void checkWrap(Object* object);
	bool checkDelete(Object* object);				//check for screen wrap

	void drawScreen(sf::RenderWindow &pWindow);		//draw to window
	void getInput();								//get keyboard input

	char* getPortNumber();							//generate random port number

	void fireBullet();								//fire bullet from ship
	void spawnAsteroid();							//spawn asteroid




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


	

//###BOX2D INIT########################################################################################################
	initPhysics();




//###WAIT UNTIL CONNECTED TO RUN GAME LOOP#############################################################################
	while (!mpClient->getConnected()) { mpClient->update(); }



	player = new Ship(mpClient->getFirstConnected());





//###RUN PROGRAM######################################################################################################
	while (window.isOpen())
	{
//###UPDATE CLIENT####################################################################################################
		mpClient->update();

//###UPDATE PHYSICS###################################################################################################
		updatePhysics();

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

		const char* serverIP   = argv[1];
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

//physics
void initPhysics()
{
	physicsWorld->SetAllowSleeping(true);
	physicsWorld->SetContactListener(new ContactListener());
}
void updatePhysics()
{
	for (int32 i = 0; i < 60; i++)
	{
		physicsWorld->Step(PHYSICS::WORLD::timeStep(), PHYSICS::WORLD::velocityIterations, PHYSICS::WORLD::positionIterations);
		checkWrap(player);
	}

	player->getSprite()->setPosition(sf::Vector2f(player->getBody()->GetPosition().x, player->getBody()->GetPosition().y));
	player->getSprite()->setRotation(player->getBody()->GetAngle()*180.0f / 3.14159f);

	for (unsigned int i = 0; i < playerBullets.size(); i++)
	{
		if (playerBullets[i] != NULL && playerBullets[i]->shouldDelete()) 
		{
			delete(playerBullets[i]);
			playerBullets[i] = NULL;
		}

		if (playerBullets[i] != NULL)
		{
			playerBullets[i]->getSprite()->setPosition(sf::Vector2f(playerBullets[i]->getBody()->GetPosition().x, playerBullets[i]->getBody()->GetPosition().y));
			playerBullets[i]->getSprite()->setRotation(playerBullets[i]->getBody()->GetAngle()*180.0f / 3.14159f);

			if (checkDelete(playerBullets[i]))
			{
				delete(playerBullets[i]);
				playerBullets[i] = NULL;
			}
		}
	}

	

	for (unsigned int i = 0; i < asteroids.size(); i++)
	{
		if (asteroids[i] != NULL)
		{
			asteroids[i]->getSprite()->setPosition(sf::Vector2f(asteroids[i]->getBody()->GetPosition().x, asteroids[i]->getBody()->GetPosition().y));
			asteroids[i]->getSprite()->setRotation(asteroids[i]->getBody()->GetAngle()*180.0f / 3.14159f);

			checkWrap(asteroids[i]);
		}
	}
}

//check for wrap/delete based on screen coordinates
void checkWrap(Object* object)
{
	b2Vec2 position = object->getBody()->GetPosition();

	if (position.x > SCREEN_WIDTH + object->getSprite()->getLocalBounds().width / 2.0f)
	{
		object->getBody()->SetTransform(b2Vec2(-object->getSprite()->getLocalBounds().width / 2.0f,
											   object->getBody()->GetPosition().y),
										object->getBody()->GetAngle());
	}
	else if (position.x < -object->getSprite()->getLocalBounds().width / 2.0f)
	{
		object->getBody()->SetTransform(b2Vec2(SCREEN_WIDTH + object->getSprite()->getLocalBounds().width / 2.0f,
											   object->getBody()->GetPosition().y),
										object->getBody()->GetAngle());
	}
	if (position.y > SCREEN_HEIGHT + object->getSprite()->getLocalBounds().height / 2.0f)
	{
		object->getBody()->SetTransform(b2Vec2(object->getBody()->GetPosition().x,
											   -object->getSprite()->getLocalBounds().height / 2.0f),
										object->getBody()->GetAngle());
	}
	else if (position.y < -object->getSprite()->getLocalBounds().height / 2.0f)
	{
		object->getBody()->SetTransform(b2Vec2(object->getBody()->GetPosition().x,
											   SCREEN_HEIGHT + object->getSprite()->getLocalBounds().height / 2.0f),
										object->getBody()->GetAngle());
	}
}
bool checkDelete(Object* object)
{
	bool needsDeleting = false;
	b2Vec2 position = object->getBody()->GetPosition();

	     if (position.x > SCREEN_WIDTH)                                  { needsDeleting = true; }
	else if (position.x < -object->getSprite()->getLocalBounds().width)  { needsDeleting = true; }
	     if (position.y > SCREEN_HEIGHT)                                 { needsDeleting = true; }
	else if (position.y < -object->getSprite()->getLocalBounds().height) { needsDeleting = true; }

	//should be abstracted
	/*PHYSICS::BULLET::data* temp = static_cast<PHYSICS::BULLET::data*>(object->body->GetUserData());
	if (temp->shouldDelete) { needsDeleting = true; }*/

	return needsDeleting;
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

	pWindow.draw(*player->getSprite());

	for (unsigned int i = 0; i < playerBullets.size(); i++)
	{
		if (playerBullets[i] != NULL)
		{
			pWindow.draw(*playerBullets[i]->getSprite());
		}
	}

	for (unsigned int i = 0; i < asteroids.size(); i++)
	{
		if (asteroids[i] != NULL)
		{
			pWindow.draw(*asteroids[i]->getSprite());
		}
	}

	//update the display
	pWindow.display();
};

//get keyboard/mouse input
void getInput()
{
	//up arrow key or W is pressed down
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		player->getBody()->SetLinearVelocity(b2Vec2(player->getBody()->GetLinearVelocity().x + player->getLinearSpeed()*cos(player->getBody()->GetAngle()),
			player->getBody()->GetLinearVelocity().y + player->getLinearSpeed()*sin(player->getBody()->GetAngle())));

		//float32 temp = player->getBody()->GetLinearVelocity().x + player->getLinearSpeed()*cos(player->getBody()->GetAngle());
		//printf("%f\n", temp);
	}

	//down arrow key or S is pressed down
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {}

	//left arrow key or A is pressed down
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		player->getBody()->SetAngularVelocity(player->getBody()->GetAngularVelocity() - player->getAngularSpeed());
		player->getSprite()->setRotation(player->getBody()->GetAngle() * 180.0f / 3.14159f);
	}

	//right arrow key or D is pressed down
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		player->getBody()->SetAngularVelocity(player->getBody()->GetAngularVelocity() + player->getAngularSpeed());
		player->getSprite()->setRotation(player->getBody()->GetAngle() * 180.0f / 3.14159f);
	}

	//space key is pressed down
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) 
	{
		if (!isSpacePressed)
		{
			fireBullet();
			isSpacePressed = true;
		}
	}
	else
	{
		isSpacePressed = false;
	}

	//p key is pressed down
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
	{
		if (!isPPressed)
		{
			spawnAsteroid();
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

//fire bullet from player
void fireBullet()
{
	for (unsigned int i = 0; i < playerBullets.size(); i++)
	{
		if (playerBullets[i] == NULL)
		{
			playerBullets[i] = new Bullet(mpClient->getFirstConnected(), player);
			break;
		}
	}
}

void spawnAsteroid()
{
	for (unsigned int i = 0; i < asteroids.size(); i++)
	{
		if (asteroids[i] == NULL)
		{
			asteroids[i] = new Asteroid();
			break;
		}
	}
}
