#include "Object.h"

Object::Object() { init(); };

Object::Object(b2World* pWorld) :mpCurrWorld(pWorld) { init(); };

Object::Object(const Object &other)
{
	body = other.body;
	sprite = other.sprite;
}

Object::~Object() { cleanup(); };

void Object::init() {}

void Object::cleanup()
{
	mShouldDelete = true;

	mpCurrWorld->DestroyBody(body);

	delete(sprite);
	sprite = NULL;
}