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


//client object
Client* mpClient;

//physics world
b2World physicsWorld(PHYSICS::WORLD::gravity());

//player object
b2Body* player;
sf::RectangleShape playerSprite = sf::RectangleShape(sf::Vector2f(10.0f, 10.0f));


//###METHOD DECLARATIONS###############################################################################################

	//init client
	Client* initClient(int argc, char** argv);

	//update physics
	void updatePhysics();

	//draw screen
	void drawScreen(sf::RenderWindow &pWindow);

	//get keyboard and mouse input
	void getInput();

	//generate random port number
	char* getPortNumber();



int main(int argc, char** argv)
{
	//create the window with global width/height and name in title bar
	sf::RenderWindow window(sf::VideoMode(static_cast<unsigned int>(SCREEN_WIDTH), static_cast<unsigned int>(SCREEN_HEIGHT)),
							"Asteroids");
	//set framerate
	window.setFramerateLimit(60);

//###INIT CLIENT#######################################################################################################
	mpClient = initClient(argc, argv);



//###BOX2D INIT########################################################################################################
	//PHYSICS::SHIP::init();
	physicsWorld.SetAllowSleeping(true);

	b2BodyDef bodyDefinition;
	bodyDefinition.type = b2_dynamicBody;
	bodyDefinition.position.Set(50.0f, 50.0f);

	player = physicsWorld.CreateBody(&bodyDefinition);

	//player->CreateFixture(&PHYSICS::SHIP::fixture());
	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(1.0f, 1.0f);

	static b2FixtureDef fix;
	fix.shape = &dynamicBox;
	fix.density = 1.0f;
	fix.friction = 0.3f;
	player->CreateFixture(&fix);
	player->SetLinearDamping(PHYSICS::SHIP::LinearDampening());
	player->SetAngularDamping(PHYSICS::SHIP::AngularDampening());

	playerSprite.setFillColor(sf::Color::Red);
	playerSprite.setOrigin(sf::Vector2f(playerSprite.getSize().x / 2.0f, playerSprite.getSize().y / 2.0f));

	

//###WAIT UNTIL CONNECTED TO RUN GAME LOOP###########################################################################
	while (!mpClient->getConnected()) { mpClient->update(); }



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

Client* initClient(int argc, char** argv)
{
	//command line arguments
	if (argc == 4) //serverIP serverPort clientPort
	{
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

		b2Vec2 position = player->GetPosition();

		if (position.x > SCREEN_WIDTH)
		{
			player->SetTransform(b2Vec2(-playerSprite.getSize().x, player->GetPosition().y), player->GetAngle());
		}
		else if (position.x < -playerSprite.getSize().x)
		{
			player->SetTransform(b2Vec2(SCREEN_WIDTH, player->GetPosition().y), player->GetAngle());
		}

		if (position.y > SCREEN_HEIGHT)
		{
			player->SetTransform(b2Vec2(player->GetPosition().x, -playerSprite.getSize().y), player->GetAngle());
		}
		else if (position.y < -playerSprite.getSize().y)
		{
			player->SetTransform(b2Vec2(player->GetPosition().x, SCREEN_HEIGHT), player->GetAngle());
		}

		playerSprite.setPosition(sf::Vector2f(position.x, position.y));
		playerSprite.setRotation(player->GetAngle()*180.0f/3.14159f);
	}
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
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {}
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