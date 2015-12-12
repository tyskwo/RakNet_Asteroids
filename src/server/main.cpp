#include "Server.h"
#include <SFML\Graphics.hpp>
#include <iostream>

int main(int argc, char** argv)
{
	Server* mpServer;

	if (argc == 2) { mpServer = new Server(argv[1]); } //create server with input port number
	else		   { mpServer = new Server("200"); }   //create server with port number of 200
	
	sf::RenderWindow window(sf::VideoMode(static_cast<unsigned int>(SCREEN_WIDTH), static_cast<unsigned int>(SCREEN_HEIGHT)), "Asteroids");
	window.setFramerateLimit(60);

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

	//run the program as long as the server is running
	while (true)
	{ 
		//mpServer->update(); 

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

		window.clear(sf::Color::White);

		//draw
		window.draw(rect);

		window.display();
	}

	return 0;
}