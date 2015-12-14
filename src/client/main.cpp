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
#include "../common/PhysicsStructs.h"



//to determine file paths of assets
bool mIsCommandLine = false;
std::string mBuildType = "";

//client object
Client* mpClient;

//font
sf::Font mFont;


//game objects
OBJECT player;
std::array<OBJECT*, 16> playerBullets;
std::array<OBJECT*, 64> asteroids;


//textures
sf::Texture TEXTURES::mFirstShip,   TEXTURES::mSecondShip,
			TEXTURES::mFirstBullet, TEXTURES::mSecondBullet,
			TEXTURES::mSmallAsteroid1,  TEXTURES::mSmallAsteroid2,  TEXTURES::mSmallAsteroid3,
			TEXTURES::mMediumAsteroid1, TEXTURES::mMediumAsteroid2, TEXTURES::mMediumAsteroid3,
			TEXTURES::mLargeAsteroid1,  TEXTURES::mLargeAsteroid2;


























//WHERE IN THE WORLD SHOULD I PUT THIS THING!!!!!!!!!
bool isSpacePressed = false;
bool isPPressed = false;





















//###METHOD DECLARATIONS###############################################################################################

	//init client
	Client* initClient(int argc, char** argv);

	//init font and window before connecting
	void initFont();
	void initWindow(sf::RenderWindow &pWindow);

	//init graphics
	void initGraphics(bool firstPlayer);

	void initPhysics();

	//update physics
	void updatePhysics();

	//check for screen wrap
	void checkWrap(OBJECT object);
	void checkWrap(OBJECT* object);
	bool checkDelete(OBJECT* object);

	//draw screen
	void drawScreen(sf::RenderWindow &pWindow);

	//get keyboard and mouse input
	void getInput();

	//generate random port number
	char* getPortNumber();

	//fire bullet from ship
	void fireBullet();

	//spawn asteroid
	void spawnAsteroid();



int main(int argc, char** argv)
{
	srand(time(NULL));

	//if to determine if being run from Visual Studio
#ifdef _DEBUG
	mBuildType = "Debug/";
#else
	mBuildType = "Release/";
#endif

	//create the window with global width/height and name in title bar
	sf::RenderWindow window(sf::VideoMode(static_cast<unsigned int>(SCREEN_WIDTH), static_cast<unsigned int>(SCREEN_HEIGHT)),
							"Asteroids");
	//set framerate
	window.setFramerateLimit(60);

	initFont();
	initWindow(window);




//###INIT CLIENT#######################################################################################################
	mpClient = initClient(argc, argv);
	TEXTURES::init(mBuildType);


	

//###WAIT UNTIL CONNECTED TO RUN GAME LOOP###########################################################################
	while (!mpClient->getConnected()) { mpClient->update(); }




//###BOX2D INIT########################################################################################################
	initPhysics();




//###INIT GRAPHICS#####################################################################################################
	initGraphics(mpClient->getFirstConnected());




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
		mIsCommandLine = true;

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
	physicsWorld.SetAllowSleeping(true);
	physicsWorld.SetContactListener(new ContactListener());

	b2BodyDef bodyDefinition;
	bodyDefinition.type = b2_dynamicBody;
	bodyDefinition.position.Set(50.0f, 50.0f);

	player.body = physicsWorld.CreateBody(&bodyDefinition);

	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(float32(TEXTURES::mFirstShip.getSize().x / 2.0f), float32(TEXTURES::mFirstShip.getSize().y / 2.0f));
	static b2FixtureDef fix;
	fix.shape = &dynamicBox;
	fix.density = PHYSICS::SHIP::density();
	fix.friction = PHYSICS::SHIP::friction();

	if (mpClient->getFirstConnected())
	{
		fix.filter.categoryBits = PHYSICS::category::FIRST_SHIP;
		fix.filter.maskBits = PHYSICS::category::ASTEROID | PHYSICS::category::SECOND_BULLET;
	}
	else
	{
		fix.filter.categoryBits = PHYSICS::category::SECOND_SHIP;
		fix.filter.maskBits = PHYSICS::category::ASTEROID | PHYSICS::category::FIRST_BULLET;
	}


	player.body->CreateFixture(&fix);
	player.body->SetLinearDamping(PHYSICS::SHIP::LinearDamping());
	player.body->SetAngularDamping(PHYSICS::SHIP::AngularDamping());
}
void updatePhysics()
{
	for (int32 i = 0; i < 60; i++)
	{
		physicsWorld.Step(PHYSICS::WORLD::timeStep(), PHYSICS::WORLD::velocityIterations, PHYSICS::WORLD::positionIterations);
		checkWrap(player);
	}

	player.sprite.setPosition(sf::Vector2f(player.body->GetPosition().x, player.body->GetPosition().y));
	player.sprite.setRotation(player.body->GetAngle()*180.0f / 3.14159f);

	for (unsigned int i = 0; i < playerBullets.size(); i++)
	{
		if (playerBullets[i] != NULL)
		{
			playerBullets[i]->sprite.setPosition(sf::Vector2f(playerBullets[i]->body->GetPosition().x, playerBullets[i]->body->GetPosition().y));
			playerBullets[i]->sprite.setRotation(playerBullets[i]->body->GetAngle()*180.0f / 3.14159f);

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
			asteroids[i]->sprite.setPosition(sf::Vector2f(asteroids[i]->body->GetPosition().x, asteroids[i]->body->GetPosition().y));
			asteroids[i]->sprite.setRotation(asteroids[i]->body->GetAngle()*180.0f / 3.14159f);

			checkWrap(asteroids[i]);
		}
	}
}

//check for wrap/delete based on screen coordinates
void checkWrap(OBJECT object)
{
	b2Vec2 position = object.body->GetPosition();

	if (position.x > SCREEN_WIDTH + object.sprite.getLocalBounds().width / 2.0f)
	{
		object.body->SetTransform(b2Vec2(-object.sprite.getLocalBounds().width / 2.0f, object.body->GetPosition().y), object.body->GetAngle());
	}
	else if (position.x < -object.sprite.getLocalBounds().width / 2.0f)
	{
		object.body->SetTransform(b2Vec2(SCREEN_WIDTH + object.sprite.getLocalBounds().width / 2.0f, object.body->GetPosition().y), object.body->GetAngle());
	}
	if (position.y > SCREEN_HEIGHT + object.sprite.getLocalBounds().height / 2.0f)
	{
		object.body->SetTransform(b2Vec2(object.body->GetPosition().x, -object.sprite.getLocalBounds().height / 2.0f), object.body->GetAngle());
	}
	else if (position.y < -object.sprite.getLocalBounds().height / 2.0f)
	{
		object.body->SetTransform(b2Vec2(object.body->GetPosition().x, SCREEN_HEIGHT + object.sprite.getLocalBounds().height / 2.0f), object.body->GetAngle());
	}
}
void checkWrap(OBJECT* object)
{
	b2Vec2 position = object->body->GetPosition();

	if (position.x > SCREEN_WIDTH + object->sprite.getLocalBounds().width / 2.0f)
	{
		object->body->SetTransform(b2Vec2(-object->sprite.getLocalBounds().width / 2.0f, object->body->GetPosition().y), object->body->GetAngle());
	}
	else if (position.x < -object->sprite.getLocalBounds().width / 2.0f)
	{
		object->body->SetTransform(b2Vec2(SCREEN_WIDTH + object->sprite.getLocalBounds().width / 2.0f, object->body->GetPosition().y), object->body->GetAngle());
	}
	if (position.y > SCREEN_HEIGHT + object->sprite.getLocalBounds().height / 2.0f)
	{
		object->body->SetTransform(b2Vec2(object->body->GetPosition().x, -object->sprite.getLocalBounds().height / 2.0f), object->body->GetAngle());
	}
	else if (position.y < -object->sprite.getLocalBounds().height / 2.0f)
	{
		object->body->SetTransform(b2Vec2(object->body->GetPosition().x, SCREEN_HEIGHT + object->sprite.getLocalBounds().height / 2.0f), object->body->GetAngle());
	}
}
bool checkDelete(OBJECT* object)
{
	bool needsDeleting = false;
	b2Vec2 position = object->body->GetPosition();

	     if (position.x > SCREEN_WIDTH)                            { needsDeleting = true; }
	else if (position.x < -object->sprite.getLocalBounds().width)  { needsDeleting = true; }
	     if (position.y > SCREEN_HEIGHT)                           { needsDeleting = true; }
	else if (position.y < -object->sprite.getLocalBounds().height) { needsDeleting = true; }

	//should be abstracted
	PHYSICS::BULLET::data* temp = static_cast<PHYSICS::BULLET::data*>(object->body->GetUserData());
	if (temp->shouldDelete) { needsDeleting = true; }

	if (needsDeleting)
	{
		physicsWorld.DestroyBody(object->body);
	}

	return needsDeleting;
}

//init window, etc.
void initFont()
{
	mFont.loadFromFile(mBuildType + "font.otf");
}
void initWindow(sf::RenderWindow &pWindow)
{
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
void initGraphics(bool firstPlayer)
{
	if (mIsCommandLine)
	{
		mBuildType = "";
	}

	if (firstPlayer)
	{
		player.sprite.setTexture(TEXTURES::mFirstShip);
		player.body->SetTransform(b2Vec2(SCREEN_WIDTH / 4.0f, SCREEN_HEIGHT / 2.0f), player.body->GetAngle());
	}
	else
	{
		player.sprite.setTexture(TEXTURES::mSecondShip);
		player.sprite.setRotation(180);
		player.body->SetTransform(b2Vec2(SCREEN_WIDTH / 4.0f * 3.0f, SCREEN_HEIGHT / 2.0f), PI);
	}
	player.sprite.setOrigin(sf::Vector2f(player.sprite.getLocalBounds().width / 2.0f, player.sprite.getLocalBounds().height / 2.0f));
}

//draw the client's game info
void drawScreen(sf::RenderWindow &pWindow)
{
	//clear the display
	pWindow.clear(sf::Color(37, 37, 37));

	pWindow.draw(player.sprite);

	for (unsigned int i = 0; i < playerBullets.size(); i++)
	{
		if (playerBullets[i] != NULL)
		{
			pWindow.draw(playerBullets[i]->sprite);
		}
	}

	for (unsigned int i = 0; i < asteroids.size(); i++)
	{
		if (asteroids[i] != NULL)
		{
			pWindow.draw(asteroids[i]->sprite);
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
		player.body->SetLinearVelocity(b2Vec2(player.body->GetLinearVelocity().x + PHYSICS::SHIP::linearSpeed()*cos(player.body->GetAngle()),
			                             player.body->GetLinearVelocity().y + PHYSICS::SHIP::linearSpeed()*sin(player.body->GetAngle())));
	}

	//down arrow key or S is pressed down
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {}

	//left arrow key or A is pressed down
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		player.body->SetAngularVelocity(player.body->GetAngularVelocity() - PHYSICS::SHIP::rotateSpeed());
		player.sprite.setRotation(player.body->GetAngle() * 180.0f / 3.14159f);
	}

	//right arrow key or D is pressed down
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		player.body->SetAngularVelocity(player.body->GetAngularVelocity() + PHYSICS::SHIP::rotateSpeed());
		player.sprite.setRotation(player.body->GetAngle() * 180.0f / 3.14159f);
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
			playerBullets[i] = new OBJECT();

			if (mpClient->getFirstConnected())
			{
				playerBullets[i]->sprite.setTexture(TEXTURES::mFirstBullet);
			}
			else			
			{
				playerBullets[i]->sprite.setTexture(TEXTURES::mSecondBullet);
			}

			b2BodyDef bodyDefinition;
			bodyDefinition.type = b2_dynamicBody;
			bodyDefinition.position.Set(player.body->GetPosition().x, player.body->GetPosition().y);

			playerBullets[i]->body = physicsWorld.CreateBody(&bodyDefinition);
			playerBullets[i]->body->SetTransform(player.body->GetPosition(), player.body->GetAngle());

			b2PolygonShape dynamicBox;
			dynamicBox.SetAsBox(float32(TEXTURES::mFirstBullet.getSize().x / 2.0f), float32(TEXTURES::mFirstBullet.getSize().y / 2.0f));

			static b2FixtureDef fix;
			fix.shape = &dynamicBox;
			fix.density = PHYSICS::BULLET::density();
			fix.friction = PHYSICS::BULLET::friction();
			if (mpClient->getFirstConnected())
			{
				fix.filter.categoryBits = PHYSICS::category::FIRST_BULLET;
				fix.filter.maskBits = PHYSICS::category::ASTEROID | PHYSICS::category::SECOND_SHIP;
			}
			else
			{
				fix.filter.categoryBits = PHYSICS::category::SECOND_BULLET;
				fix.filter.maskBits = PHYSICS::category::ASTEROID | PHYSICS::category::FIRST_SHIP;
			}

			playerBullets[i]->body->CreateFixture(&fix);
			playerBullets[i]->body->SetLinearVelocity(b2Vec2(PHYSICS::BULLET::linearSpeed()*cos(player.body->GetAngle()),
				                                             PHYSICS::BULLET::linearSpeed()*sin(player.body->GetAngle())));

			playerBullets[i]->sprite.setOrigin(sf::Vector2f(playerBullets[i]->sprite.getLocalBounds().width / 2.0f, 
				                                            playerBullets[i]->sprite.getLocalBounds().height / 2.0f));

			playerBullets[i]->sprite.setPosition(sf::Vector2f(playerBullets[i]->body->GetPosition().x, playerBullets[i]->body->GetPosition().y));
			playerBullets[i]->sprite.setRotation(playerBullets[i]->body->GetAngle()*180.0f / 3.14159f);

			PHYSICS::BULLET::data* temp = new PHYSICS::BULLET::data();
			temp->index = i;
			playerBullets[i]->body->SetUserData(temp);


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
			asteroids[i] = new OBJECT();
			b2CircleShape circleShape;


			int randomSize = rand() % 3;
			if (randomSize == 0) // small
			{
				int randomType = rand() % 3;

				if (randomType == 0) 
				{
					asteroids[i]->sprite.setTexture(TEXTURES::mSmallAsteroid1); 
					circleShape.m_radius = 25;
				}
				else if (randomType == 1) 
				{ 
					asteroids[i]->sprite.setTexture(TEXTURES::mSmallAsteroid2);
					circleShape.m_radius = 25;
				}
				else if (randomType == 2) 
				{
					asteroids[i]->sprite.setTexture(TEXTURES::mSmallAsteroid3);
					circleShape.m_radius = 25;
				}
			}
			else if (randomSize == 1) // medium
			{
				int randomType = rand() % 3;

				if (randomType == 0)
				{
					asteroids[i]->sprite.setTexture(TEXTURES::mMediumAsteroid1);
					circleShape.m_radius = 50;
				}
				else if (randomType == 1)
				{
					asteroids[i]->sprite.setTexture(TEXTURES::mMediumAsteroid2);
					circleShape.m_radius = 50;
				}
				else if (randomType == 2)
				{
					asteroids[i]->sprite.setTexture(TEXTURES::mMediumAsteroid3);
					circleShape.m_radius = 50;
				}
			}
			else if (randomSize == 2) // large
			{
				int randomType = rand() % 2;

				if (randomType == 0)
				{
					asteroids[i]->sprite.setTexture(TEXTURES::mLargeAsteroid1);
					circleShape.m_radius = 100;
				}
				else if (randomType == 1)
				{
					asteroids[i]->sprite.setTexture(TEXTURES::mLargeAsteroid2);
					circleShape.m_radius = 100;
				}
			}


			b2Vec2 position;
			position.x = SCREEN_WIDTH / 2.0f;
			position.y = SCREEN_HEIGHT / 2.0f;
			float32 angle = (static_cast <float32> (rand()) / static_cast <float32> (RAND_MAX)) * PI * 2;


			b2BodyDef bodyDefinition;
			bodyDefinition.type = b2_dynamicBody;
			bodyDefinition.position.Set(position.x, position.y);

			asteroids[i]->body = physicsWorld.CreateBody(&bodyDefinition);
			asteroids[i]->body->SetTransform(position, angle);


			static b2FixtureDef fix;
			fix.shape = &circleShape;
			fix.density = PHYSICS::ASTEROIDS::density();
			fix.filter.categoryBits = PHYSICS::category::ASTEROID;
			fix.filter.maskBits = PHYSICS::category::FIRST_SHIP | PHYSICS::category::SECOND_SHIP | PHYSICS::category::FIRST_BULLET | PHYSICS::category::SECOND_BULLET | PHYSICS::category::ASTEROID;
			asteroids[i]->body->CreateFixture(&fix);


			asteroids[i]->body->SetLinearVelocity(b2Vec2(PHYSICS::ASTEROIDS::linearSpeed()*cos(asteroids[i]->body->GetAngle()),
				                                         PHYSICS::ASTEROIDS::linearSpeed()*sin(asteroids[i]->body->GetAngle())));


			//set sprite's properties
			asteroids[i]->sprite.setOrigin(sf::Vector2f(asteroids[i]->sprite.getLocalBounds().width  / 2.0f,
				                                        asteroids[i]->sprite.getLocalBounds().height / 2.0f));

			asteroids[i]->sprite.setPosition(sf::Vector2f(asteroids[i]->body->GetPosition().x, 
				                                          asteroids[i]->body->GetPosition().y));

			asteroids[i]->sprite.setRotation(asteroids[i]->body->GetAngle()*180.0f / 3.14159f);

			break;
		}
	}
}
