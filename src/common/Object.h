#ifndef OBJECT_H
#define OBJECT_H

//library includes
#include "Box2D\Box2D.h"
#include "SFML\Graphics.hpp"
#include "..\common\Trackable.h"

//forward declarations
class b2Body;
class b2World;

class Object : public Trackable
{
public:
	Object();
	Object(b2World* pWorld);
	Object(const Object &other);
	~Object();

	void init();
	virtual void cleanup();

	inline bool        shouldDelete() { return mShouldDelete; };
	inline sf::Sprite* getSprite()    { return sprite; };
	inline b2Body*     getBody()      { return body;   };


	inline void setDelete(bool trash) { mShouldDelete = trash; };

	inline void setShouldDeleteBody(bool value) { mShouldDeleteBody = value; };

protected:
	void initPhysics();
	void initSprite();


	b2Body* body;
	sf::Sprite* sprite;

	b2World* mpWorld;

	bool mShouldDeleteBody;
	bool mShouldDelete = false;
};

#endif