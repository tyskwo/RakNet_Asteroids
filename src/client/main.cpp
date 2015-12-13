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
#include "../common/GameStructs.h"



//to determine file paths of assets
bool mIsCommandLine = false;
std::string mBuildType = "";

//client object
Client* mpClient;

//font
sf::Font mFont;

//physics world
b2World physicsWorld(PHYSICS::WORLD::gravity());

//player object
b2Body* player;
sf::Sprite playerSprite;

//textures
sf::Texture TEXTURES::mFirstShip,   TEXTURES::mSecondShip,
			TEXTURES::mFirstBullet, TEXTURES::mSecondBullet;


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
	void checkWrap(b2Body* object);

	//draw screen
	void drawScreen(sf::RenderWindow &pWindow);

	//get keyboard and mouse input
	void getInput();

	//generate random port number
	char* getPortNumber();

	//fire bullet from ship
	void fireBullet();



int main(int argc, char** argv)
{
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

void updatePhysics()
{
	for (int32 i = 0; i < 60; i++)
	{
		physicsWorld.Step(PHYSICS::WORLD::timeStep(), PHYSICS::WORLD::velocityIterations, PHYSICS::WORLD::positionIterations);

		checkWrap(player);
	}

	playerSprite.setPosition(sf::Vector2f(player->GetPosition().x, player->GetPosition().y));
	playerSprite.setRotation(player->GetAngle()*180.0f / 3.14159f);
}

void checkWrap(b2Body* body)
{
	b2Vec2 position = body->GetPosition();

	if (position.x > SCREEN_WIDTH)
	{
		body->SetTransform(b2Vec2(-playerSprite.getLocalBounds().width, body->GetPosition().y), body->GetAngle());
	}
	else if (position.x < -playerSprite.getLocalBounds().width)
	{
		body->SetTransform(b2Vec2(SCREEN_WIDTH, body->GetPosition().y), body->GetAngle());
	}

	if (position.y > SCREEN_HEIGHT)
	{
		body->SetTransform(b2Vec2(body->GetPosition().x, -playerSprite.getLocalBounds().height), body->GetAngle());
	}
	else if (position.y < -playerSprite.getLocalBounds().height)
	{
		body->SetTransform(b2Vec2(body->GetPosition().x, SCREEN_HEIGHT), body->GetAngle());
	}
}

void initFont()
{
	mFont.loadFromFile(mBuildType + "font.otf");
}

void initWindow(sf::RenderWindow &pWindow)
{
	pWindow.clear(sf::Color::Black);

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
		playerSprite.setTexture(TEXTURES::mFirstShip);
		player->SetTransform(b2Vec2(SCREEN_WIDTH / 4.0f, SCREEN_HEIGHT / 2.0f), player->GetAngle());
	}
	else
	{
		playerSprite.setTexture(TEXTURES::mSecondShip);
		playerSprite.setRotation(180);
		player->SetTransform(b2Vec2(SCREEN_WIDTH / 4.0f * 3.0f, SCREEN_HEIGHT / 2.0f), PI);
	}
	playerSprite.setOrigin(sf::Vector2f(playerSprite.getLocalBounds().width / 2.0f, playerSprite.getLocalBounds().height / 2.0f));
}

//draw the client's game info
void drawScreen(sf::RenderWindow &pWindow)
{
	//clear the display
	pWindow.clear(sf::Color::Black);

	pWindow.draw(playerSprite);

	//update the display
	pWindow.display();
};

//get keyboard/mouse input
void getInput()
{
	//up arrow key or W is pressed down
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		player->SetLinearVelocity(b2Vec2(player->GetLinearVelocity().x + PHYSICS::SHIP::linearSpeed()*cos(player->GetAngle()),
			                             player->GetLinearVelocity().y + PHYSICS::SHIP::linearSpeed()*sin(player->GetAngle())));
	}

	//down arrow key or S is pressed down
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {}

	//left arrow key or A is pressed down
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		player->SetAngularVelocity(player->GetAngularVelocity() - PHYSICS::SHIP::rotateSpeed());
		playerSprite.setRotation(player->GetAngle() * 180.0f / 3.14159f);
	}

	//right arrow key or D is pressed down
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		player->SetAngularVelocity(player->GetAngularVelocity() + PHYSICS::SHIP::rotateSpeed());
		playerSprite.setRotation(player->GetAngle() * 180.0f / 3.14159f);
	}

	//space key is pressed down
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) 
	{
		fireBullet();
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

void fireBullet()
{

}

void initPhysics()
{
	physicsWorld.SetAllowSleeping(true);

	b2BodyDef bodyDefinition;
	bodyDefinition.type = b2_dynamicBody;
	bodyDefinition.position.Set(50.0f, 50.0f);

	player = physicsWorld.CreateBody(&bodyDefinition);

	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(TEXTURES::mFirstShip.getSize().x, TEXTURES::mFirstShip.getSize().y);

	static b2FixtureDef fix;
	fix.shape    = &dynamicBox;
	fix.density  = PHYSICS::SHIP::density();
	fix.friction = PHYSICS::SHIP::friction();

	player->CreateFixture(&fix);
	player->SetLinearDamping(PHYSICS::SHIP::LinearDampening());
	player->SetAngularDamping(PHYSICS::SHIP::AngularDampening());
}