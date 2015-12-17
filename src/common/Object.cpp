#include "Object.h"

//library includes
#include "Box2D\Box2D.h"

Object::Object() { init(); };

Object::Object(b2World* pWorld) :mpWorld(pWorld) { init(); };

Object::Object(const Object &other)
{
	body = other.body;
	sprite = other.sprite;
	mShouldDeleteBody = true;
}

Object::~Object() { };//cleanup(); };

void Object::init() { mShouldDeleteBody = false; }

void Object::cleanup()
{
	mShouldDelete = true;
 
	/*if (body != NULL)
	{
		mpWorld->DestroyBody(body);
	}*/

	delete(sprite);
	sprite = NULL;
}