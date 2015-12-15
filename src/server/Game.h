#ifndef GAME_H
#define GAME_H


//library includes
#include "Box2D\Box2D.h"
#include "SFML\Graphics.hpp"

//game includes
#include "../common/Ship.h"
#include "../common/Bullet.h"
#include "../common/Asteroid.h"

#include <array>

class Game
{
public:
	Game();
	~Game();

	void init();
	void cleanup();

	void update();

private:
	b2World* physicsWorld;

	Ship* firstPlayer, *secondPlayer;
	std::array<Bullet*, 8> firstPlayerBullets, secondPlayerBullets;
	std::array<Asteroid*, 64> asteroids;

	void checkWrap(Object* object);
	bool checkDelete(Object* object);
	void checkAsteroidSpawn(Asteroid* asteroid);
};

#endif