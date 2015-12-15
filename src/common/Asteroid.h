#ifndef ASTEROID_H
#define ASTEROID_H

//game includes
#include "Object.h"

enum Size
{
	SMALL,
	MEDIUM,
	LARGE,
};

struct AsteroidSpawn
{
	bool deleteSelf    = false;
	int  numberToSpawn = 0;
	int  sizeToSpawn   = 0;

	bool operator==(const AsteroidSpawn& rhs)
	{
		return deleteSelf    == rhs.deleteSelf    &&
			   numberToSpawn == rhs.numberToSpawn &&
			   sizeToSpawn   == rhs.sizeToSpawn;
	}

	bool operator!=(const AsteroidSpawn& rhs)
	{
		return deleteSelf != rhs.deleteSelf       ||
			   numberToSpawn != rhs.numberToSpawn ||
			   sizeToSpawn != rhs.sizeToSpawn;
	}
};
const static AsteroidSpawn NoSpawn;

class Asteroid : public Object
{
public:
	Asteroid();
	Asteroid(b2World* pWorld);
	Asteroid(int size, b2Vec2 location, b2World* pWorld);
	Asteroid(const Asteroid &other, b2World* pWorld);
	~Asteroid();

	void init();

	inline const float32 getLinearSpeed() { return mPhysics::linearSpeed(); };
	inline const float32 getRotateSpeed() { return mPhysics::rotateSpeed(); };
	
	inline Size getSize()   { return size;   };
	inline int  getHealth() { return health; };

	void loseHealth(int damage);

	inline AsteroidSpawn getSpawn()       { return collisionSpawn; };
	inline void setSpawn(AsteroidSpawn x) { collisionSpawn = x;    };

private:
	void initSize();
	void initPhysics(b2Vec2 location);
	void initSprite();

	struct mPhysics
	{
		static const float32 linearSpeed() { return 0.4f; };
		static const float32 rotateSpeed() { return 0.012f; };
		static const float32 density()	   { return 2.0f; };
	};

	Size size;
	int health;

	AsteroidSpawn collisionSpawn = NoSpawn;
};

#endif