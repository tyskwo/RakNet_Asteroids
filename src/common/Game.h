#ifndef GAME_H
#define GAME_H

class Ship;
class Bullet;
class Asteroid;
class Object;

class b2World;

#include <array>
#include "..\common\Trackable.h"
#include "NetworkStructs.h"

class Game : public Trackable
{
public:
	Game(bool isServer);
	~Game();

	void init(bool isServer);
	void cleanup();

	void update();

	int fireBullet(bool isFirstPlayer);
	void spawnAsteroid();

	inline Ship* getFirstPlayer()  { return firstPlayer;  };
	inline Ship* getSecondPlayer() { return secondPlayer; };

	inline std::array<Bullet*, 8> getFirstPlayerBullets()  { return firstPlayerBullets;  };
	inline std::array<Bullet*, 8> getSecondPlayerBullets() { return secondPlayerBullets; };

	void setFirstPlayerBullets(BulletData data);
	void setSecondPlayerBullets(BulletData data);


	inline std::array<Asteroid*, 64> getAsteroids() { return asteroids; };

	inline b2World* getWorld() { return physicsWorld; };

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