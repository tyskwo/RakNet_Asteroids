//SFML includes
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

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

//input counter
InputCounter mCounter;


//###METHOD DECLARATIONS###############################################################################################

	//init client
	Client* initClient(int argc, char** argv);

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

	//while trying to connect, don't update the game logic
	while (!mpClient->getConnected()) { mpClient->update(); }


//###BOX2D STUFF######################################################################################################
	b2Vec2 gravity(0.0f, 0.0f);
	b2World world(gravity);
	world.SetAllowSleeping(true);

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(0.0f, 4.0f);
	b2Body* body = world.CreateBody(&bodyDef);

	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(1.0f, 1.0f);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.3f;

	body->CreateFixture(&fixtureDef);

	float32 timeStep = 1.0f / 60.0f;
	int32 velocityIterations = 6;
	int32 positionIterations = 2;


	sf::RectangleShape rect = sf::RectangleShape(sf::Vector2f(10.0f, 10.0f));
	rect.setFillColor(sf::Color::Red);

	body->SetLinearDamping(0.0025f);
	body->SetAngularDamping(0.1f);

	/* THIS CODE GOES INSIDE THE UPDATE LOOP SOMEWHERE
	for (int32 i = 0; i < 60; i++)
	{
		world.Step(timeStep, velocityIterations, positionIterations);

		b2Vec2 position = body->GetPosition();

		if (position.x > SCREEN_WIDTH)
		{
			body->SetTransform(b2Vec2(-rect.getSize().x, body->GetPosition().y), body->GetAngle());
		}
		else if (position.x < -rect.getSize().x)
		{
			body->SetTransform(b2Vec2(SCREEN_WIDTH, body->GetPosition().y), body->GetAngle());
		}

		if (position.y > SCREEN_HEIGHT)
		{
			body->SetTransform(b2Vec2(body->GetPosition().x, -rect.getSize().y), body->GetAngle());
		}
		else if (position.y < -rect.getSize().y)
		{
			body->SetTransform(b2Vec2(body->GetPosition().x, SCREEN_HEIGHT), body->GetAngle());
		}

		float32 angle = body->GetAngle();
		rect.setPosition(sf::Vector2f(position.x, position.y));
		rect.setRotation(angle);
	}


	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		body->SetLinearVelocity(b2Vec2(body->GetLinearVelocity().x + 0.1f, body->GetLinearVelocity().y));

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		body->SetLinearVelocity(b2Vec2(body->GetLinearVelocity().x - 0.1f, body->GetLinearVelocity().y));

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		body->SetLinearVelocity(b2Vec2(body->GetLinearVelocity().x, body->GetLinearVelocity().y - 0.1f));

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		body->SetLinearVelocity(b2Vec2(body->GetLinearVelocity().x, body->GetLinearVelocity().y + 0.1f));

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		body->SetAngularVelocity(body->GetAngularVelocity() + 0.1f);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		body->SetAngularVelocity(body->GetAngularVelocity() - 0.1f);

	*/


//###RUN PROGRAM######################################################################################################
	while (window.isOpen())
	{
		//update client
		mpClient->update();

//###GET INPUT########################################################################################################
		getInput();
		printf("%d\n", mpClient->getInputCounter().counterUP);
		
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

//draw the client's game info
void drawScreen(sf::RenderWindow &pWindow)
{
	//clear the display
	pWindow.clear(sf::Color::Black);

	//update the display
	pWindow.display();
};

//get keyboard/mouse input
void getInput()
{
	//up arrow key or W is pressed down
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		mCounter.counterUP++;
	}
	//down arrow key or S is pressed down
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S))
	{
		mCounter.counterDOWN++;
	}
	//left arrow key or A is pressed down
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		mCounter.counterLEFT++;
	}
	//right arrow key or D is pressed down
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		mCounter.counterRIGHT++;
	}
	//space key is pressed down
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
	{
		mCounter.counterSPACE++;
	}

	mpClient->setInputCounter(mCounter);
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