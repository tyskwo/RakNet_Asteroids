#ifndef ASTEROID_H
#define ASTEROID_H


//library includes

//game includes
#include "Object.h"


enum Size
{
	SMALL,
	MEDIUM,
	LARGE,
};

class Asteroid : public Object
{
public:
	Asteroid();
	Asteroid(const Asteroid &other);
	~Asteroid();

	void init();

	inline const float32 getLinearSpeed() { return mPhysics::linearSpeed(); };
	inline const float32 getRotateSpeed() { return mPhysics::rotateSpeed(); };

private:
	void initSize();
	void initPhysics();
	void initSprite();

	struct mPhysics
	{
		static const float32 linearSpeed() { return 0.1f; };
		static const float32 rotateSpeed() { return 0.008f; };
		static const float32 density()	   { return 2.0f; };
	};

	Size size;
};

#endif