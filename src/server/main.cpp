#include <SFML/Graphics.hpp>
#include "Server.h"

int main()
{
	// create the window
	sf::RenderWindow window(sf::VideoMode(800, 600), "PONGBREAK server");
	window.setFramerateLimit(60);


	Server* mpServer = new Server("200");

	// run the program as long as the window is open
	while (window.isOpen())
	{
		mpServer->update();

		// check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;
		while (window.pollEvent(event))
		{
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
				window.close();

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
				window.close();
		}

		// clear the window with black color
		window.clear(sf::Color::Black);

		// end the current frame
		window.display();
	}

	return 0;
}