#ifndef OBJECT_H
#define OBJECT_H


//library includes
#include "Box2D\Box2D.h"
#include "SFML\Graphics.hpp"

//game includes
#include "../common/PhysicsStructs.h"

class Object
{
public:
	Object();
	Object(b2World* pWorld);
	Object(const Object &other);
	~Object();

	void init();
	void cleanup();

	inline bool        shouldDelete() { return mShouldDelete; };
	inline sf::Sprite* getSprite()    { return sprite; };
	inline b2Body*     getBody()      { return body;   };


	inline void setDelete(bool trash) { mShouldDelete = trash; };

protected:
	void initPhysics();
	void initSprite();


	b2Body* body;
	sf::Sprite* sprite;

	b2World* mpCurrWorld;

	bool mShouldDelete = false;
};

#endif