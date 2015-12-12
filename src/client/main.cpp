//SFML includes
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

//library includes
#include <iostream>
#include <sstream>
#include <string>

//game includes
#include "Client.h"
#include "../common/GameStructs.h"


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
	
	Client* mpClient = initClient(argc, argv);

	//while trying to connect, don't update the game logic
	while (!mpClient->getConnected()) { mpClient->update(); }



//###RUN PROGRAM######################################################################################################
	while (window.isOpen())
	{
		//update client
		mpClient->update();

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

//draw the client's game info
void drawScreen(sf::RenderWindow &pWindow)
{
	//clear the display
	pWindow.clear(sf::Color::White);

	//update the display
	pWindow.display();
};

//get keyboard/mouse input
void getInput()
{
	//up arrow key is pressed down
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
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