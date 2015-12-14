#include "Object.h"

Object::Object() { init(); };

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

	physicsWorld->DestroyBody(body);

	delete(sprite);
	sprite = NULL;
}