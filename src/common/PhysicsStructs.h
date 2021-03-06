#ifndef PHYSICSSTRUCTS_H
#define PHYSICSSTRUCTS_H

//so there is no redefinition errors
#pragma once

//Box2D includes
#include <Box2D\Box2D.h>

//SFML includes
#include <SFML/Graphics.hpp>

//library includes
#include <cstdio>



//#####################################################################################################################
//#  FILE FOR PHYSICS STRUCTS, VALUES, AND METHODS BETWEEN THE SERVER AND CLIENT  #####################################
//#####################################################################################################################



//constants
const float SCREEN_WIDTH  = 1200.0;
const float SCREEN_HEIGHT = 768.0;
const float PI = std::atan(1.0f) * 4.0f;



//struct for textures
struct TEXTURES
{
	static sf::Texture mFirstShip,   mSecondShip,
					   mFirstBullet, mSecondBullet,
					   mSmallAsteroid1,  mSmallAsteroid2,  mSmallAsteroid3,
					   mMediumAsteroid1, mMediumAsteroid2, mMediumAsteroid3,
					   mLargeAsteroid1,  mLargeAsteroid2,  mLargeAsteroid3,
					   mMarker;

	static void init(std::string mBuildType)
	{
		mFirstShip.loadFromFile(mBuildType + "redShip.png");
		mSecondShip.loadFromFile(mBuildType + "blueShip.png");

		mFirstBullet.loadFromFile(mBuildType + "redBullet.png");
		mSecondBullet.loadFromFile(mBuildType + "blueBullet.png");

		mSmallAsteroid1.loadFromFile(mBuildType + "asteroidS1.png");
		mSmallAsteroid2.loadFromFile(mBuildType + "asteroidS2.png");
		mSmallAsteroid3.loadFromFile(mBuildType + "asteroidS3.png");

		mMediumAsteroid1.loadFromFile(mBuildType + "asteroidM1.png");
		mMediumAsteroid2.loadFromFile(mBuildType + "asteroidM2.png");
		mMediumAsteroid3.loadFromFile(mBuildType + "asteroidM3.png");

		mLargeAsteroid1.loadFromFile(mBuildType + "asteroidL1.png");
		mLargeAsteroid2.loadFromFile(mBuildType + "asteroidL2.png");
		mLargeAsteroid3.loadFromFile(mBuildType + "asteroidL3.png");

		mMarker.loadFromFile(mBuildType + "marker.png");
	}
};



//struct for physics values
struct PHYSICS
{
	enum category 
	{
		FIRST_SHIP    = 0x0001,
		SECOND_SHIP   = 0x0002,
		ASTEROID      = 0x0004,
		FIRST_BULLET  = 0x0008,
		SECOND_BULLET = 0x0010,
	};

	//physics world
	struct WORLD
	{
		static const b2Vec2  gravity()	{ return b2Vec2(0.0f, 0.0f); };
		static const float32 timeStep()	{ return 1.0f / 60.0f; };

		static const int32   velocityIterations = 6;
		static const int32   positionIterations = 2;
	};
};



//physics world
extern b2World* gpPhysicsWorld;
static b2World* getWorld() { return gpPhysicsWorld; };



class ServerContactListener: public b2ContactListener
{
	void BeginContact(b2Contact* contact);
};

class ClientContactListener : public b2ContactListener
{
	void BeginContact(b2Contact* contact);
};

#endif