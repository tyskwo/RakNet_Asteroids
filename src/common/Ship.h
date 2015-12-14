#ifndef SHIP_H
#define SHIP_H


//library includes
/*#include "Box2D\Box2D.h"
#include "SFML\Graphics.hpp"*/

//game includes
#include "Object.h"

class Ship: public Object
{
public:
	Ship();
	Ship(bool firstConnected);
	~Ship();

	void init(bool firstConnected);

	inline const float32 getLinearSpeed()  { return mPhysics::linearSpeed(); };
	inline const float32 getAngularSpeed() { return mPhysics::rotateSpeed(); };

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
};

#endif