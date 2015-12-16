#include "Game.h"

#include "..\common\PhysicsStructs.h"

//game includes
#include "..\common\Ship.h"
#include "..\common\Bullet.h"
#include "..\common\Asteroid.h"


Game::Game(bool isServer) { init(isServer); };

Game::~Game() { cleanup(); };

void Game::init(bool isServer)
{
	mHasStarted = false;

	physicsWorld = new b2World(PHYSICS::WORLD::gravity());
	physicsWorld->SetAllowSleeping(true);
	if (isServer) physicsWorld->SetContactListener(new ServerContactListener());
	else          physicsWorld->SetContactListener(new ClientContactListener());


	firstPlayer  = new Ship(true,  physicsWorld);
	secondPlayer = new Ship(false, physicsWorld);

	for (unsigned int i = 0; i < firstPlayerBullets.size(); i++)
	{
		firstPlayerBullets[i]  = NULL;
		secondPlayerBullets[i] = NULL;
	}

	for (unsigned int i = 0; i < mAsteroids.size(); i++)
	{
		mAsteroids[i] = NULL;
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

	for (unsigned int i = 0; i < mAsteroids.size(); i++)
	{
		if (mAsteroids[i] != NULL)
		{
			delete mAsteroids[i];
			mAsteroids[i] = NULL;
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
	firstPlayer->getSprite()->setPosition(sf::Vector2f(firstPlayer->getBody()->GetPosition().x, firstPlayer->getBody()->GetPosition().y));
	firstPlayer->getSprite()->setRotation(firstPlayer->getBody()->GetAngle()*180.0f / 3.14159f);
	secondPlayer->getSprite()->setPosition(sf::Vector2f(secondPlayer->getBody()->GetPosition().x, secondPlayer->getBody()->GetPosition().y));
	secondPlayer->getSprite()->setRotation(secondPlayer->getBody()->GetAngle()*180.0f / 3.14159f);


	for (unsigned int i = 0; i < firstPlayerBullets.size(); i++)
	{
		if (firstPlayerBullets[i] != NULL && firstPlayerBullets[i]->shouldDelete())
		{
			delete(firstPlayerBullets[i]);
			firstPlayerBullets[i] = NULL;
		}
		if (firstPlayerBullets[i] != NULL)
		{
			firstPlayerBullets[i]->getSprite()->setPosition(sf::Vector2f(firstPlayerBullets[i]->getBody()->GetPosition().x, firstPlayerBullets[i]->getBody()->GetPosition().y));
			firstPlayerBullets[i]->getSprite()->setRotation(firstPlayerBullets[i]->getBody()->GetAngle()*180.0f / 3.14159f);

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
			secondPlayerBullets[i]->getSprite()->setPosition(sf::Vector2f(secondPlayerBullets[i]->getBody()->GetPosition().x, secondPlayerBullets[i]->getBody()->GetPosition().y));
			secondPlayerBullets[i]->getSprite()->setRotation(secondPlayerBullets[i]->getBody()->GetAngle()*180.0f / 3.14159f);

			if (checkDelete(secondPlayerBullets[i]))
			{
				delete secondPlayerBullets[i];
				secondPlayerBullets[i] = NULL;
			}
		}
	}


	for (unsigned int i = 0; i < mAsteroids.size(); i++)
	{
		if (mAsteroids[i] != NULL)
		{
			checkAsteroidSpawn(mAsteroids[i]);
		}
		if (mAsteroids[i] != NULL && mAsteroids[i]->shouldDelete())
		{
			delete(mAsteroids[i]);
			mAsteroids[i] = NULL;
		}
		if (mAsteroids[i] != NULL)
		{
			mAsteroids[i]->getSprite()->setPosition(sf::Vector2f(mAsteroids[i]->getBody()->GetPosition().x, mAsteroids[i]->getBody()->GetPosition().y));
			mAsteroids[i]->getSprite()->setRotation(mAsteroids[i]->getBody()->GetAngle()*180.0f / 3.14159f);

			checkWrap(mAsteroids[i]);
		}
	}
}

void Game::checkWrap(Object* object)
{
	b2Vec2 position = object->getBody()->GetPosition();

	if (position.x > SCREEN_WIDTH + object->getSprite()->getLocalBounds().width / 2.0f &&
		object->getBody()->GetLinearVelocity().x > 0)
	{
		object->getBody()->SetTransform(b2Vec2(-object->getSprite()->getLocalBounds().width / 2.0f,
			object->getBody()->GetPosition().y),
			object->getBody()->GetAngle());
	}
	else if (position.x < -object->getSprite()->getLocalBounds().width / 2.0f &&
		object->getBody()->GetLinearVelocity().x < 0)
	{
		object->getBody()->SetTransform(b2Vec2(SCREEN_WIDTH + object->getSprite()->getLocalBounds().width / 2.0f,
			object->getBody()->GetPosition().y),
			object->getBody()->GetAngle());
	}
	if (position.y > SCREEN_HEIGHT + object->getSprite()->getLocalBounds().height / 2.0f &&
		object->getBody()->GetLinearVelocity().y > 0)
	{
		object->getBody()->SetTransform(b2Vec2(object->getBody()->GetPosition().x,
			-object->getSprite()->getLocalBounds().height / 2.0f),
			object->getBody()->GetAngle());
	}
	else if (position.y < -object->getSprite()->getLocalBounds().height / 2.0f &&
		object->getBody()->GetLinearVelocity().y < 0)
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

	if ((position.x > SCREEN_WIDTH                                  && object->getBody()->GetLinearVelocity().x > 0)   ||
		(position.x < -object->getSprite()->getLocalBounds().width  && object->getBody()->GetLinearVelocity().x < 0)   ||
	    (position.y > SCREEN_HEIGHT                                 && object->getBody()->GetLinearVelocity().y > 0)   ||
		(position.y < -object->getSprite()->getLocalBounds().height && object->getBody()->GetLinearVelocity().y < 0))
	{ needsDeleting = true; }

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
				for (unsigned int j = 0; j < mAsteroids.size(); j++)
				{
					if (mAsteroids[j] == NULL)
					{
						b2Vec2 location = asteroid->getBody()->GetPosition();
						float  angle = (rand() / RAND_MAX) * PI * 2.0f;
						float  radius = static_cast<float>(int(asteroid->getSize()              * int(asteroid->getSize() / 2)) * 25 + 25 +
										int(asteroid->getSpawn().sizeToSpawn * int(asteroid->getSpawn().sizeToSpawn / 2)) * 25 + 25);

						location.x += cos(angle)*radius;
						location.y += sin(angle)*radius;

						mAsteroids[j] = new Asteroid(asteroid->getSpawn().sizeToSpawn, location, physicsWorld);
						break;
					}
				}
			}
		}

		asteroid->setSpawn(NoSpawn);
	}
}

//fire bullet from player
int Game::fireBullet(bool isFirstPlayer)
{
	if (isFirstPlayer)
	{
		for (unsigned int i = 0; i < firstPlayerBullets.size(); i++)
		{
			if (firstPlayerBullets[i] == NULL)
			{
				firstPlayerBullets[i] = new Bullet(isFirstPlayer, firstPlayer, physicsWorld);
				return i;
				break;
			}
		}
	}
	else
	{
		for (unsigned int i = 0; i < secondPlayerBullets.size(); i++)
		{
			if (secondPlayerBullets[i] == NULL)
			{
				secondPlayerBullets[i] = new Bullet(isFirstPlayer, secondPlayer, physicsWorld);
				return i;
				break;
			}
		}
	}
	return -1;
}

int Game::spawnAsteroid()
{
	for (unsigned int i = 0; i < mAsteroids.size(); i++)
	{
		if (mAsteroids[i] == NULL)
		{
			mAsteroids[i] = new Asteroid(physicsWorld);
			return i;
		}
	}

	return -1;
}

void Game::setFirstPlayerBullets(BulletData data)
{
	firstPlayerBullets[data.index] = new Bullet(true, firstPlayer, physicsWorld);
}

void Game::setSecondPlayerBullets(BulletData data)
{
	secondPlayerBullets[data.index] = new Bullet(false, secondPlayer, physicsWorld);
}

void Game::setAsteroids(AsteroidData data)
{
	mAsteroids[data.index] = new Asteroid(data.size, b2Vec2(data.position.x, data.position.y), b2Vec2(data.velocity.x, data.velocity.y), data.velocity.rot, data.position.angle, data.type, physicsWorld);
}