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

	void fireBullet(bool isFirstPlayer);
	void spawnAsteroid();

	inline Ship* getFirstPlayer()  { return firstPlayer;  };
	inline Ship* getSecondPlayer() { return secondPlayer; };

	inline std::array<Bullet*, 8> getFirstPlayerBullets()  { return firstPlayerBullets;  };
	inline std::array<Bullet*, 8> getSecondPlayerBullets() { return secondPlayerBullets; };

	inline std::array<Asteroid*, 64> getAsteroids() { return asteroids; };



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