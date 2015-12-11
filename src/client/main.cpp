#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <iostream>
#include <sstream>
#include <string>

//#include "Client.h"
//#include "ObjectInfo.h"


//method declarations
void draw(sf::RenderWindow pWindow);
void getInput();

char* getPortNumber();



int main()
{
	//create the window
	const float SCREEN_WIDTH = 1200.0;
	const float SCREEN_HEIGHT = 768.0;

	sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Asteroids");
	window.setFramerateLimit(60);




//###INIT CLIENT#######################################################################################################
	//connect to a random port
	char* portNumber = getPortNumber();

	//connect to external ip
	std::string ipAddress = "";
	std::cout << "Input server IP or \"localhost\": ";
	std::cin >> ipAddress;
	//Client* mpClient = new Client(portNumber, ipAddress.c_str(), "200");

	//while trying to connect, don't update the game logic
	//while (!mpClient->getConnected()) { mpClient->update(); }





//###RUN PROGRAM######################################################################################################
	while (window.isOpen())
	{
		//update client
		//mpClient->update();

//###GET INPUT########################################################################################################
		getInput();
		
//###DRAW#############################################################################################################
		draw(window);

//###WINDOW CLOSE#####################################################################################################
		sf::Event event;
		while (window.pollEvent(event))
		{
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
				window.close();
		}
	}

	return 0;
}

//draw the client's game info
void draw(sf::RenderWindow pWindow)
{
	pWindow.clear(sf::Color::White);
	pWindow.display();
}

//get keyboard/mouse input
void getInput()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
	}
}


//get random port number from 201 - 401
char* getPortNumber()
{
	srand(static_cast<unsigned int>(time(NULL)));
	int randPortNumber = rand() % (401 - 201 + 1) + 201;

	std::stringstream converter;
	converter << randPortNumber;
	std::string temp_str = converter.str();

	return (char*)temp_str.c_str();
}