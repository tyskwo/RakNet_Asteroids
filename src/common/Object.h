#ifndef OBJECT_H
#define OBJECT_H

//library includes
#include "Box2D\Box2D.h"
#include "SFML\Graphics.hpp"

//forward declarations
class b2Body;
class b2World;

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

	b2World* mpWorld;

	bool mShouldDelete = false;
};

#endif