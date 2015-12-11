#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <iostream>
#include <sstream>
#include <string>

#include "Client.h"
#include "ObjectInfo.h"



int main()
{
	// create the window
	const float SCREEN_WIDTH = 1024.0;
	const float SCREEN_HEIGHT = 768.0;
	const float HALF_SCREEN_WIDTH = SCREEN_WIDTH / 2;
	const float HALF_SCREEN_HEIGHT = SCREEN_HEIGHT / 2;

	sf::RenderWindow window(sf::VideoMode(1024, 768), "PONGBREAK");
	window.setFramerateLimit(60);




//##############################################################INIT CLIENT############################################
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
	Client* mpClient = new Client(char_type, ipAddress.c_str(), "200");

	//while trying to connect, don't update the game logic
	while (!mpClient->getConnected()) { mpClient->update(); }





//########################################################INIT SHAPES##################################################
	sf::RectangleShape player(sf::Vector2f(20, 100));
	sf::RectangleShape opponent(sf::Vector2f(20, 100));
	sf::RectangleShape ball(sf::Vector2f(20, 20));
	ball.setPosition(HALF_SCREEN_WIDTH - 10, HALF_SCREEN_HEIGHT - 10);
	

	std::array<sf::RectangleShape, 10> hlines;
	std::array<sf::RectangleShape, 10> vlines;

	sf::View view1(sf::FloatRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT));

	for (unsigned int i = 0; i < hlines.size(); i++)
	{
		hlines[i] = sf::RectangleShape(sf::Vector2f(SCREEN_WIDTH, 2));
		vlines[i] = sf::RectangleShape(sf::Vector2f(2, SCREEN_HEIGHT));

		if (i == 0 || i == 5)
		{
			hlines[i].setFillColor(sf::Color(10, 10, 25, 100));
			vlines[i].setFillColor(sf::Color(10, 10, 25, 100));
		}
		else
		{
			hlines[i].setFillColor(sf::Color(10, 10, 25, 40));
			vlines[i].setFillColor(sf::Color(10, 10, 25, 40));
		}
	}

	std::array<sf::RectangleShape, 18> playerBricks;
	std::array<sf::RectangleShape, 18> opponentBricks;

	sf::Text playerScore;
	sf::Text opponentScore;
	
	sf::Text beginText;
	beginText.setString("WAITING...\nPLAYER 2\nPRESS SPACE");
	beginText.setOrigin(beginText.getLocalBounds().width / 2.0f, beginText.getLocalBounds().height / 2.0f);
	beginText.setPosition(10.0f, 10.0f);
	beginText.setColor(sf::Color(10, 10, 25, 100));
	beginText.setCharacterSize(95);

	sf::Font font;
	if (font.loadFromFile("Debug/font.otf"))
	{
		playerScore.setFont(font);
		opponentScore.setFont(font);
		beginText.setFont(font);

		playerScore.setCharacterSize(400);
		opponentScore.setCharacterSize(400);
	}
	else if (font.loadFromFile("Release/font.otf"))
	{
		playerScore.setFont(font);
		opponentScore.setFont(font);
		beginText.setFont(font);

		playerScore.setCharacterSize(400);
		opponentScore.setCharacterSize(400);
	}
	else if (font.loadFromFile("font.otf"))
	{
		playerScore.setFont(font);
		opponentScore.setFont(font);
		beginText.setFont(font);

		playerScore.setCharacterSize(400);
		opponentScore.setCharacterSize(400);
	}

	if (mpClient->getFirstConnected())
	{
		player.setFillColor(sf::Color(200, 10, 10));
		player.setPosition(sf::Vector2f(200.0, HALF_SCREEN_HEIGHT - 50.0f));

		opponent.setFillColor(sf::Color(10, 10, 200));
		opponent.setPosition(sf::Vector2f(SCREEN_WIDTH - 200.0f - 20.0f, HALF_SCREEN_HEIGHT - 50.0f));

		for (unsigned int i = 0; i < playerBricks.size(); i++)
		{
			playerBricks[i] = sf::RectangleShape(sf::Vector2f(20, 75));
			playerBricks[i].setPosition(10.0f + 40.0f * (i / 6), (HALF_SCREEN_HEIGHT + 25.0f - 125.0f * 3.0f) + 125.0f * (i % 6));
			playerBricks[i].setFillColor(sf::Color(200, 10, 10, 100));
		}

		for (unsigned int i = 0; i < opponentBricks.size(); i++)
		{
			opponentBricks[i] = sf::RectangleShape(sf::Vector2f(20, 75));
			opponentBricks[i].setPosition(SCREEN_WIDTH - 10.0f - 20.0f - 40.0f * (i / 6), (HALF_SCREEN_HEIGHT + 25.0f - 125.0f * 3.0f) + 125.0f * (i % 6));
			opponentBricks[i].setFillColor(sf::Color(10, 10, 200, 100));
		}

		playerScore.setString("0");
		playerScore.setOrigin(playerScore.getLocalBounds().width / 2.0f, playerScore.getLocalBounds().height / 2.0f);
		playerScore.setPosition(SCREEN_WIDTH / 4, HALF_SCREEN_HEIGHT + HALF_SCREEN_HEIGHT / 4);
		playerScore.setColor(sf::Color(200, 10, 10, 50));

		opponentScore.setString("0");
		opponentScore.setOrigin(opponentScore.getLocalBounds().width / 2.0f, opponentScore.getLocalBounds().height / 2.0f);
		opponentScore.setPosition(SCREEN_WIDTH / 4 * 3, HALF_SCREEN_HEIGHT + HALF_SCREEN_HEIGHT / 4);
		opponentScore.setColor(sf::Color(10, 10, 200, 50));
	}
	else
	{
		player.setFillColor(sf::Color(10, 10, 200));
		player.setPosition(sf::Vector2f(SCREEN_WIDTH - 200.0f - 20.0f, HALF_SCREEN_HEIGHT - 50.0f));

		opponent.setFillColor(sf::Color(200, 10, 10));
		opponent.setPosition(sf::Vector2f(200.0, HALF_SCREEN_HEIGHT - 50.0f));

		for (unsigned int i = 0; i < opponentBricks.size(); i++)
		{
			opponentBricks[i] = sf::RectangleShape(sf::Vector2f(20, 75));
			opponentBricks[i].setPosition(10.0f + 40.0f * (i / 6), (HALF_SCREEN_HEIGHT + 25.0f - 125.0f * 3.0f) + 125.0f * (i % 6));
			opponentBricks[i].setFillColor(sf::Color(200, 10, 10, 100));
		}

		for (unsigned int i = 0; i < playerBricks.size(); i++)
		{
			playerBricks[i] = sf::RectangleShape(sf::Vector2f(20, 75));
			playerBricks[i].setPosition(SCREEN_WIDTH - 10.0f - 20.0f - 40.0f * (i / 6), (HALF_SCREEN_HEIGHT + 25.0f - 125.0f * 3.0f) + 125.0f * (i % 6));
			playerBricks[i].setFillColor(sf::Color(10, 10, 200, 100));
		}

		playerScore.setString("0");
		playerScore.setOrigin(playerScore.getLocalBounds().width / 2.0f, playerScore.getLocalBounds().height / 2.0f);
		playerScore.setPosition(SCREEN_WIDTH / 4 * 3, HALF_SCREEN_HEIGHT + HALF_SCREEN_HEIGHT / 4);
		playerScore.setColor(sf::Color(10, 10, 200, 50));

		opponentScore.setString("0");
		opponentScore.setOrigin(opponentScore.getLocalBounds().width / 2.0f, opponentScore.getLocalBounds().height / 2.0f);
		opponentScore.setPosition(SCREEN_WIDTH / 4, HALF_SCREEN_HEIGHT + HALF_SCREEN_HEIGHT / 4);
		opponentScore.setColor(sf::Color(200, 10, 10, 50));
	}

	ball.setFillColor(sf::Color(10, 10, 25));
	ball.setPosition(HALF_SCREEN_WIDTH - 10, HALF_SCREEN_HEIGHT - 10);

	float yPos = HALF_SCREEN_HEIGHT - 50.0f;



//####################################################RUN PROGRAM#####################################################
	while (window.isOpen())
	{
		mpClient->update();

//#############################################GET INPUT##############################################################
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			yPos -= 5.0;
			if (yPos <= 0) yPos = 0;

			mpClient->setPaddleVelocity(-5.0f);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			yPos += 5.0;
			if (yPos >= SCREEN_HEIGHT - player.getSize().y) yPos = SCREEN_HEIGHT - player.getSize().y;

			mpClient->setPaddleVelocity(5.0f);
		}
		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			mpClient->setPaddleVelocity(0.0f);
		}
		player.setPosition(player.getPosition().x, yPos);
		mpClient->setPaddleLoc(player.getPosition().x, yPos);

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && !mpClient->getFirstConnected())
		{
			mpClient->setGameStart();
		}

//##############################################INTERPOLATE###########################################################
		if (mpClient->getNumBallTargets() < 1 && mpClient->getGameInfo().started)
		{
			mpClient->setBallPosition(ball.getPosition().x + mpClient->getGameInfo().ball.xVel, ball.getPosition().y + mpClient->getGameInfo().ball.yVel);
			ball.setPosition(mpClient->getGameInfo().ball.x, mpClient->getGameInfo().ball.y);
		}
		else if (mpClient->getGameInfo().started)
		{
			ObjectInfo binfo = mpClient->getBallInterpolation().GetNext(mpClient->getElapsedT());
			ball.setPosition(binfo.GetState().mX, binfo.GetState().mY);
			mpClient->setBallPosition(binfo.GetState().mX, binfo.GetState().mY);
		}

		if (mpClient->getNumPaddleTargets() < 1)
		{
			if (mpClient->getFirstConnected())
			{
				mpClient->setOpponentLoc(opponent.getPosition().x, opponent.getPosition().y + mpClient->getGameInfo().rPlayer.velocity);
				opponent.setPosition(sf::Vector2f(mpClient->getGameInfo().rPlayer.x, mpClient->getGameInfo().rPlayer.y));
			}
			else
			{
				mpClient->setOpponentLoc(opponent.getPosition().x, opponent.getPosition().y + mpClient->getGameInfo().lPlayer.velocity);
				opponent.setPosition(sf::Vector2f(mpClient->getGameInfo().lPlayer.x, mpClient->getGameInfo().lPlayer.y));
			}
		}
		else
		{
			ObjectInfo info = mpClient->getOpponentInterpolation().GetNext(mpClient->getElapsedT());
			if (mpClient->getFirstConnected())
			{
				opponent.setPosition(sf::Vector2f(mpClient->getGameInfo().rPlayer.x, info.GetState().mY));
			}
			else
			{
				opponent.setPosition(sf::Vector2f(mpClient->getGameInfo().lPlayer.x, info.GetState().mY));
			}
			mpClient->setOpponentLoc(opponent.getPosition().x, info.GetState().mY);
		}
		
		
			


//##############################################DRAW GRID###########################################################
		for (unsigned int i = 0; i < hlines.size(); i++)
		{
			float lineY = ball.getPosition().y - 100 * i;
			float newY = (hlines[i].getPosition().y - lineY) / (2 * (4 * i) + 2);
			
			if (i < 5)
			{
				lineY = (ball.getPosition().y + 10) - 100 * i;
				newY = (hlines[i].getPosition().y - lineY) / (2 * (4 * i) + 2);
			}
			else
			{
				lineY = (ball.getPosition().y + 10) + 100 * (i - 5);
				newY = (hlines[i].getPosition().y - lineY) / (2 * (4 * (i - 5)) + 2);
			}
			
			hlines[i].setPosition(hlines[i].getPosition().x, hlines[i].getPosition().y - newY);
		}

		for (unsigned int i = 0; i < vlines.size(); i++)
		{
			float lineX = (ball.getPosition().x + 10) - 100 * i;
			float newX = (vlines[i].getPosition().x - lineX) / (2 * (4 * i) + 2);

			if (i < 5)
			{
				lineX = (ball.getPosition().x + 10) - 100 * i;
				newX = (vlines[i].getPosition().x - lineX) / (2 * (4 * i) + 2);
			}
			else
			{
				lineX = (ball.getPosition().x + 10) + 100 * (i - 5);
				newX = (vlines[i].getPosition().x - lineX) / (2 * (4 * (i - 5)) + 2);
			}

			vlines[i].setPosition(vlines[i].getPosition().x - newX, vlines[i].getPosition().y);
		}


//##############################################UPDATE GOAL TEXT#####################################################		
		if (mpClient->getFirstConnected())
		{
			playerScore.setString(std::to_string(mpClient->getGameInfo().lPlayer.goalsScored));
			opponentScore.setString(std::to_string(mpClient->getGameInfo().rPlayer.goalsScored));
		}
		else
		{
			playerScore.setString(std::to_string(mpClient->getGameInfo().rPlayer.goalsScored));
			opponentScore.setString(std::to_string(mpClient->getGameInfo().lPlayer.goalsScored));
		}	

		opponentScore.setOrigin(opponentScore.getLocalBounds().width / 2.0f, opponentScore.getLocalBounds().height / 2.0f);
		playerScore.setOrigin(playerScore.getLocalBounds().width / 2.0f, playerScore.getLocalBounds().height / 2.0f);



//###############################################WINDOW CLOSE#########################################################
		sf::Event event;
		while (window.pollEvent(event))
		{
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
				window.close();
		}





//################################################CLEAR AND DRAW######################################################
		window.clear(sf::Color::White);

		view1.setRotation((ball.getPosition().x - HALF_SCREEN_WIDTH) / 125.0f);
		window.setView(view1);

		for (unsigned int i = 0; i < vlines.size(); i++)
		{
			window.draw(hlines[i]);
			window.draw(vlines[i]);
		}

		if (!mpClient->getGameInfo().started)
		{
			window.draw(beginText);

			if (mpClient->getGameInfo().finished)
			{
				if (mpClient->getGameInfo().lPlayer.goalsScored > mpClient->getGameInfo().rPlayer.goalsScored) beginText.setString("P1 WINS...\nPLAYER 2\nPRESS SPACE");
				else beginText.setString("P2 WINS...\nPLAYER 2\nPRESS SPACE");
			}
			else
			{
				beginText.setString("WAITING...\nPLAYER 2\nPRESS SPACE");
			}
		}

		for (unsigned int i = 0; i < playerBricks.size(); i++)
		{
			if (mpClient->getFirstConnected())
			{
				if (mpClient->getGameInfo().lPlayer.bricks[i % 6][i / 6] == true)
				{
					window.draw(playerBricks[i]);
				}
			}
			else
			{
				if (mpClient->getGameInfo().rPlayer.bricks[i % 6][i / 6] == true)
				{
					window.draw(playerBricks[i]);
				}
			}
		}

		for (unsigned int i = 0; i < opponentBricks.size(); i++)
		{
			if (mpClient->getFirstConnected())
			{
				if (mpClient->getGameInfo().rPlayer.bricks[i % 6][i / 6] == true)
				{
					window.draw(opponentBricks[i]);
				}
			}
			else
			{
				if (mpClient->getGameInfo().lPlayer.bricks[i % 6][i / 6] == true)
				{
					window.draw(opponentBricks[i]);
				}
			}
		}

		window.draw(player);
		window.draw(opponent);
		window.draw(ball);
		window.draw(playerScore);
		window.draw(opponentScore);

		window.display();
	}

	return 0;
}