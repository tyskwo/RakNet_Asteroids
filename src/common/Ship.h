#ifndef SHIP_H
#define SHIP_H

//game includes
#include "Object.h"

class Ship: public Object
{
public:
	Ship();
	Ship(bool firstConnected, b2World* pWorld);
	~Ship();

	void init(bool firstConnected);

	inline const float32 getLinearSpeed()  { return mPhysics::linearSpeed(); };
	inline const float32 getAngularSpeed() { return mPhysics::rotateSpeed(); };

	inline int  getHealth()            { return health;    };
	inline void loseHealth(int damage) { health -= damage; };

private:
	void initPhysics(bool firstConnected);
	void initSprite(bool  firstConnected);

	struct mPhysics
	{
		static const float32 LinearDamping()   { return 0.025f; };
		static const float32 AngularDamping()  { return 0.1f; };
		static const float32 linearSpeed()	   { return 0.1f; };
		static const float32 rotateSpeed()	   { return 0.008f; };
		static const float32 density()		   { return 1.0f; };
		static const float32 friction()	       { return 0.3f; };
	};

	int health = 20;
	//bool mIsVisible; //will this ship need a sprite, or is it simply data for the server to use for physics predictions?
					 //technically doesn't need a sprite, but we're not drawing it anywhere, so it doesn't really matter.
};

#endif