#include "Game.h"

Game::Game() { init(); };

Game::~Game() { cleanup(); };

void Game::init()
{
	physicsWorld = new b2World(PHYSICS::WORLD::gravity());
	physicsWorld->SetAllowSleeping(true);
	physicsWorld->SetContactListener(new ContactListener());

	firstPlayer  = new Ship(true,  physicsWorld);
	secondPlayer = new Ship(false, physicsWorld);

	for (unsigned int i = 0; i < firstPlayerBullets.size(); i++)
	{
		firstPlayerBullets[i]  = NULL;
		secondPlayerBullets[i] = NULL;
	}

	for (unsigned int i = 0; i < asteroids.size(); i++)
	{
		asteroids[i] = NULL;
	}
};

void Game::cleanup()
{
	delete firstPlayer;
	firstPlayer = NULL;

	delete secondPlayer;
	secondPlayer = NULL;

	for (unsigned int i = 0; i < firstPlayerBullets.size(); i++)
	{
		if (firstPlayerBullets[i] != NULL)
		{
			delete firstPlayerBullets[i];
			firstPlayerBullets[i] = NULL;
		}
	}

	for (unsigned int i = 0; i < secondPlayerBullets.size(); i++)
	{
		if (secondPlayerBullets[i] != NULL)
		{
			delete secondPlayerBullets[i];
			secondPlayerBullets[i] = NULL;
		}
	}

	for (unsigned int i = 0; i < asteroids.size(); i++)
	{
		if (asteroids[i] != NULL)
		{
			delete asteroids[i];
			asteroids[i] = NULL;
		}
	}
}

void Game::update()
{
	for (int32 i = 0; i < 60; i++)
	{
		physicsWorld->Step(PHYSICS::WORLD::timeStep(), PHYSICS::WORLD::velocityIterations, PHYSICS::WORLD::positionIterations);
		checkWrap(firstPlayer);
		checkWrap(secondPlayer);
	}

	for (unsigned int i = 0; i < firstPlayerBullets.size(); i++)
	{
		if (firstPlayerBullets[i] != NULL && firstPlayerBullets[i]->shouldDelete())
		{
			delete(firstPlayerBullets[i]);
			firstPlayerBullets[i] = NULL;
		}
		if (firstPlayerBullets[i] != NULL)
		{
			if (checkDelete(firstPlayerBullets[i]))
			{
				delete firstPlayerBullets[i];
				firstPlayerBullets[i] = NULL;
			}
		}
	}

	for (unsigned int i = 0; i < secondPlayerBullets.size(); i++)
	{
		if (secondPlayerBullets[i] != NULL && secondPlayerBullets[i]->shouldDelete())
		{
			delete(secondPlayerBullets[i]);
			secondPlayerBullets[i] = NULL;
		}
		if (secondPlayerBullets[i] != NULL)
		{
			if (checkDelete(secondPlayerBullets[i]))
			{
				delete secondPlayerBullets[i];
				secondPlayerBullets[i] = NULL;
			}
		}
	}


	for (unsigned int i = 0; i < asteroids.size(); i++)
	{
		if (asteroids[i] != NULL)
		{
			checkAsteroidSpawn(asteroids[i]);
		}
		if (asteroids[i] != NULL && asteroids[i]->shouldDelete())
		{
			delete(asteroids[i]);
			asteroids[i] = NULL;
		}
		if (asteroids[i] != NULL)
		{
			checkWrap(asteroids[i]);
		}
	}
}

void Game::checkWrap(Object* object)
{
	b2Vec2 position = object->getBody()->GetPosition();

	if (position.x > SCREEN_WIDTH + object->getSprite()->getLocalBounds().width / 2.0f)
	{
		object->getBody()->SetTransform(b2Vec2(-object->getSprite()->getLocalBounds().width / 2.0f,
			object->getBody()->GetPosition().y),
			object->getBody()->GetAngle());
	}
	else if (position.x < -object->getSprite()->getLocalBounds().width / 2.0f)
	{
		object->getBody()->SetTransform(b2Vec2(SCREEN_WIDTH + object->getSprite()->getLocalBounds().width / 2.0f,
			object->getBody()->GetPosition().y),
			object->getBody()->GetAngle());
	}
	if (position.y > SCREEN_HEIGHT + object->getSprite()->getLocalBounds().height / 2.0f)
	{
		object->getBody()->SetTransform(b2Vec2(object->getBody()->GetPosition().x,
			-object->getSprite()->getLocalBounds().height / 2.0f),
			object->getBody()->GetAngle());
	}
	else if (position.y < -object->getSprite()->getLocalBounds().height / 2.0f)
	{
		object->getBody()->SetTransform(b2Vec2(object->getBody()->GetPosition().x,
			SCREEN_HEIGHT + object->getSprite()->getLocalBounds().height / 2.0f),
			object->getBody()->GetAngle());
	}
}

bool Game::checkDelete(Object* object)
{
	bool needsDeleting = false;
	b2Vec2 position = object->getBody()->GetPosition();

	if (position.x > SCREEN_WIDTH)                                       { needsDeleting = true; }
	else if (position.x < -object->getSprite()->getLocalBounds().width)  { needsDeleting = true; }
	if (position.y > SCREEN_HEIGHT)                                      { needsDeleting = true; }
	else if (position.y < -object->getSprite()->getLocalBounds().height) { needsDeleting = true; }

	return needsDeleting;
}

void Game::checkAsteroidSpawn(Asteroid* asteroid)
{
	if (asteroid->getSpawn() != NoSpawn)
	{
		if (asteroid->getSpawn().deleteSelf)
		{
			asteroid->setDelete(true);
		}

		if (asteroid->getSpawn().numberToSpawn > 0)
		{
			for (int i = 0; i < asteroid->getSpawn().numberToSpawn; i++)
			{
				for (unsigned int j = 0; j < asteroids.size(); j++)
				{
					if (asteroids[j] == NULL)
					{
						b2Vec2 location = asteroid->getBody()->GetPosition();
						float  angle = (rand() / RAND_MAX) * PI * 2.0f;
						float  radius = int(asteroid->getSize()              * int(asteroid->getSize() / 2)) * 25 + 25 +
							int(asteroid->getSpawn().sizeToSpawn * int(asteroid->getSpawn().sizeToSpawn / 2)) * 25 + 25;

						location.x += cos(angle)*radius;
						location.y += sin(angle)*radius;

						asteroids[j] = new Asteroid(asteroid->getSpawn().sizeToSpawn, location, physicsWorld);
						break;
					}
				}
			}
		}

		asteroid->setSpawn(NoSpawn);
	}
}