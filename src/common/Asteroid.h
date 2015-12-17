#ifndef ASTEROID_H
#define ASTEROID_H

//game includes
#include "Object.h"
#include "..\common\NetworkStructs.h"

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
	Asteroid(int size, b2Vec2 position, b2Vec2 velocity, float rotVel, float angle, int type, b2World* pWorld);
	Asteroid(int size, b2Vec2 location, b2World* pWorld);
	Asteroid(const Asteroid &other, b2World* pWorld);
	~Asteroid();

	virtual void cleanup(bool shouldDelete = true);

	void init();
	void initAlreadyCreated(int size, b2Vec2 position, b2Vec2 velocity, float rotVel, float angle, int type);

	inline const float32 getLinearSpeed() { return mPhysics::linearSpeed(); };
	inline const float32 getRotateSpeed() { return mPhysics::rotateSpeed(); };
	
	inline Size     getSize()     { return size;   };
	inline int      getHealth()   { return health; };
	position getPosition();

	void loseHealth(int damage);

	inline AsteroidSpawn getSpawn()       { return collisionSpawn; };
	inline void setSpawn(AsteroidSpawn x) { collisionSpawn = x;    };

	bool isOnScreen();

	void interpolate(RakNet::Time currentTime, RakNet::Time onePacketAgo, RakNet::Time twoPacketAgo);
	void addInterpolation(AsteroidObject data);
	inline bool isFinishedInterpolating() { return isDoneInterpolating; };
	inline void setDoneInterpolated(bool yes) { isDoneInterpolating = yes; };

private:
	void initSize();
	void initPhysics(b2Vec2 location);
	void initSprite(int type = 0);

	struct mPhysics
	{
		static const float32 linearSpeed() { return 0.4f; };
		static const float32 rotateSpeed() { return 0.012f; };
		static const float32 density()	   { return 2.0f; };
	};

	Size size;
	int health;

	AsteroidSpawn collisionSpawn = NoSpawn;


	struct InterpolationObject
	{
		RakNet::Time timeStamp;
		AsteroidObject   data;
	};
	InterpolationObject targetState, startState;
	bool isDoneInterpolating = true;

	float startTime;
	float targetTime;
};

#endif