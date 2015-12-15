#include "Ship.h"

Ship::Ship(bool firstConnected, b2World* pWorld) : Object(pWorld) { init(firstConnected); };

Ship::~Ship() { cleanup(); };

void Ship::init(bool firstConnected)
{
	initPhysics(firstConnected);
	initSprite(firstConnected);
}

void Ship::initPhysics(bool firstConnected)
{
	b2BodyDef bodyDefinition;
	bodyDefinition.type = b2_dynamicBody;
	bodyDefinition.position.Set(0.0f, 0.0f);

	body = mpWorld->CreateBody(&bodyDefinition);

	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(float32(TEXTURES::mFirstShip.getSize().x / 2.0f), float32(TEXTURES::mFirstShip.getSize().y / 2.0f));

	static b2FixtureDef fix;
	fix.shape = &dynamicBox;
	fix.density = mPhysics::density();
	fix.friction = mPhysics::friction();

	if (firstConnected)
	{
		fix.filter.categoryBits = PHYSICS::category::FIRST_SHIP;
		fix.filter.maskBits = PHYSICS::category::ASTEROID | PHYSICS::category::SECOND_BULLET;
	}
	else
	{
		fix.filter.categoryBits = PHYSICS::category::SECOND_SHIP;
		fix.filter.maskBits = PHYSICS::category::ASTEROID | PHYSICS::category::FIRST_BULLET;
	}


	body->CreateFixture(&fix);
	body->SetLinearDamping(mPhysics::LinearDamping());
	body->SetAngularDamping(mPhysics::AngularDamping());

	body->SetUserData(this);
}

void Ship::initSprite(bool firstConnected)
{
	sprite = new sf::Sprite();
	if (firstConnected)
	{
		sprite->setTexture(TEXTURES::mFirstShip);
		body->SetTransform(b2Vec2(SCREEN_WIDTH / 4.0f, SCREEN_HEIGHT / 2.0f), body->GetAngle());
	}
	else
	{
		sprite->setTexture(TEXTURES::mSecondShip);
		sprite->setRotation(180);
		body->SetTransform(b2Vec2(SCREEN_WIDTH / 4.0f * 3.0f, SCREEN_HEIGHT / 2.0f), PI);
	}

	sprite->setOrigin(sf::Vector2f(sprite->getLocalBounds().width / 2.0f, sprite->getLocalBounds().height / 2.0f));
	sprite->setPosition(sf::Vector2f(body->GetPosition().x, body->GetPosition().y));
	sprite->setRotation(body->GetAngle()*180.0f / 3.14159f);
}