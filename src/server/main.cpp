#include "Server.h"

#include <SFML/Graphics.hpp>

#include "..\common\MemoryTracker.h"
#include "..\common\PhysicsStructs.h"
#include "..\common\Game.h"
#include <array>

sf::Texture TEXTURES::mFirstShip,       TEXTURES::mSecondShip,
            TEXTURES::mFirstBullet,     TEXTURES::mSecondBullet,
            TEXTURES::mSmallAsteroid1,  TEXTURES::mSmallAsteroid2,  TEXTURES::mSmallAsteroid3,
            TEXTURES::mMediumAsteroid1, TEXTURES::mMediumAsteroid2, TEXTURES::mMediumAsteroid3,
            TEXTURES::mLargeAsteroid1,  TEXTURES::mLargeAsteroid2,  TEXTURES::mLargeAsteroid3;

std::string mBuildType;

int main(int argc, char** argv)
{
#ifdef _DEBUG
	mBuildType = "Debug/";
#else
	mBuildType = "Release/";
#endif
	if (argc == 2) { mBuildType = ""; }
	TEXTURES::init(mBuildType);

	Server* mpServer;
	

	if (argc == 2) { mpServer = new Server(argv[1]); } //create server with input port number
	else		   { mpServer = new Server("200");   } //create server with port number of 200

	while (true) //run the program as long as the server is running
	{
		mpServer->update();

		for (unsigned int i = 0; i < mpServer->mpGames.size(); i++) { mpServer->mpGames[i]->update(); }
	}

	gMemoryTracker.reportAllocations(std::cout);

	system("pause");

	return 0;
}