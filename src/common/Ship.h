#ifndef SHIP_H
#define SHIP_H

//game includes
#include "Object.h"
#include "..\common\NetworkStructs.h"



class Ship: public Object
{
public:
	Ship();
	Ship(bool firstConnected, b2World* pWorld);
	~Ship();

	virtual void cleanup();

	void init(bool firstConnected);

	inline const float32 getLinearSpeed()  { return mPhysics::linearSpeed(); };
	inline const float32 getAngularSpeed() { return mPhysics::rotateSpeed(); };

	inline int  getHealth()            { return health;    };
	void loseHealth(int damage);

	void interpolate(RakNet::Time currentTime, RakNet::Time onePacketAgo, RakNet::Time twoPacketAgo);
	void addInterpolation(RakNet::Time timeStamp, ShipObject data);
	inline bool isFinishedInterpolating() { return isDoneInterpolating; };
	inline void setDoneInterpolated(bool yes) { isDoneInterpolating = yes; };

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

	int health = 300;
	


	struct InterpolationObject
	{
		RakNet::Time timeStamp;
		ShipObject   data;
	};
	InterpolationObject targetState, startState;
	bool isDoneInterpolating = true;

	float startTime;
	float targetTime;
};

#endif