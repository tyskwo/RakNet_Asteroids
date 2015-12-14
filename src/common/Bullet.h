#ifndef BULLET_H
#define BULLET_H


//library includes

//game includes
#include "Object.h"
#include "Ship.h"

class Bullet: public Object
{
public:
	Bullet();
	Bullet(const Bullet &other);
	Bullet(bool firstConnected, Ship* player);
	~Bullet();

	void init(bool firstConnected, Ship* player);

	inline const float32 getLinearSpeed()  { return mPhysics::linearSpeed(); };

private:
	void initPhysics(bool firstConnected, Ship* player);
	void initSprite(bool  firstConnected);

	struct mPhysics
	{
		static const float32 linearSpeed() { return 10.0f; };
		static const float32 density()	   { return 1.0f; };
		static const float32 friction()	   { return 0.3f; };
	};
};

#endif