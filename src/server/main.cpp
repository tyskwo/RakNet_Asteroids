#include "Server.h"

#include "SFML\Graphics.hpp"
#include "Box2D\Box2D.h"
#include "../common/PhysicsStructs.h"

b2World* physicsWorld = new b2World(PHYSICS::WORLD::gravity());

sf::Texture TEXTURES::mFirstShip,		TEXTURES::mSecondShip,
			TEXTURES::mFirstBullet,		TEXTURES::mSecondBullet,
			TEXTURES::mSmallAsteroid1,  TEXTURES::mSmallAsteroid2,  TEXTURES::mSmallAsteroid3,
			TEXTURES::mMediumAsteroid1, TEXTURES::mMediumAsteroid2, TEXTURES::mMediumAsteroid3,
			TEXTURES::mLargeAsteroid1,  TEXTURES::mLargeAsteroid2,  TEXTURES::mLargeAsteroid3;

int main(int argc, char** argv)
{
	Server* mpServer;

	if (argc == 2) { mpServer = new Server(argv[1]); } //create server with input port number
	else		   { mpServer = new Server("200");   } //create server with port number of 200

	while (true) { mpServer->update(); } //run the program as long as the server is running

	return 0;
}