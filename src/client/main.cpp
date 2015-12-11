#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <iostream>
#include <sstream>
#include <string>

//#include "Client.h"
//#include "ObjectInfo.h"



int main()
{
	// create the window
	const float SCREEN_WIDTH = 1024.0;
	const float SCREEN_HEIGHT = 768.0;
	const float HALF_SCREEN_WIDTH = SCREEN_WIDTH / 2;
	const float HALF_SCREEN_HEIGHT = SCREEN_HEIGHT / 2;

	sf::RenderWindow window(sf::VideoMode(1200, 768), "Asteroids");
	window.setFramerateLimit(60);




//###INIT CLIENT#######################################################################################################
	//connect to a random port
	srand(static_cast<unsigned int>(time(NULL)));
	int randPortNumber = rand() % (301 - 201 + 1) + 201;
	std::stringstream converter;
	converter << randPortNumber;
	std::string temp_str = converter.str();
	char* char_type = (char*)temp_str.c_str();

	//connect to external ip
	std::string ipAddress = "";
	std::cout << "Input server IP or \"localhost\": ";
	std::cin >> ipAddress;
	//Client* mpClient = new Client(char_type, ipAddress.c_str(), "200");

	//while trying to connect, don't update the game logic
	//while (!mpClient->getConnected()) { mpClient->update(); }





//###RUN PROGRAM######################################################################################################
	while (window.isOpen())
	{
		//mpClient->update();

//###GET INPUT########################################################################################################
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
		}

//###DRAW#############################################################################################################
		/**/



//###CLEAR AND DRAW###################################################################################################
		window.clear(sf::Color::White);
		window.display();

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