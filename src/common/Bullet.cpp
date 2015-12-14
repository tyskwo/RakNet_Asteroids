#include "Bullet.h"


Bullet::Bullet(bool firstConnected, Ship* player) { init(firstConnected, player); };

Bullet::Bullet(const Bullet &other)
{
	body = other.body;
	sprite = other.sprite;
}

Bullet::~Bullet() {};

void Bullet::init(bool firstConnected, Ship* player)
{
	initPhysics(firstConnected, player);
	initSprite(firstConnected);
}

void Bullet::initPhysics(bool firstConnected, Ship* player)
{
	b2BodyDef bodyDefinition;
	bodyDefinition.type = b2_dynamicBody;
	bodyDefinition.position.Set(player->getBody()->GetPosition().x, player->getBody()->GetPosition().y);

	body = physicsWorld->CreateBody(&bodyDefinition);
	body->SetTransform(player->getBody()->GetPosition(), player->getBody()->GetAngle());

	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(float32(TEXTURES::mFirstBullet.getSize().x / 2.0f), float32(TEXTURES::mFirstBullet.getSize().y / 2.0f));

	static b2FixtureDef fix;
	fix.shape = &dynamicBox;
	fix.density = mPhysics::density();
	fix.friction = mPhysics::friction();

	if (firstConnected)
	{
		fix.filter.categoryBits = PHYSICS::category::FIRST_BULLET;
		fix.filter.maskBits     = PHYSICS::category::ASTEROID | PHYSICS::category::SECOND_SHIP;
	}
	else
	{
		fix.filter.categoryBits = PHYSICS::category::SECOND_BULLET;
		fix.filter.maskBits     = PHYSICS::category::ASTEROID | PHYSICS::category::FIRST_SHIP;
	}

	body->CreateFixture(&fix);
	body->SetLinearVelocity(b2Vec2(mPhysics::linearSpeed()*cos(player->getBody()->GetAngle()),
		                           mPhysics::linearSpeed()*sin(player->getBody()->GetAngle())));

	body->SetUserData(this);
}

void Bullet::initSprite(bool firstConnected)
{
	sprite = new sf::Sprite();
	if (firstConnected)
	{
		sprite->setTexture(TEXTURES::mFirstBullet);
	}
	else
	{
		sprite->setTexture(TEXTURES::mSecondBullet);
	}

	sprite->setOrigin(sf::Vector2f(sprite->getLocalBounds().width / 2.0f, sprite->getLocalBounds().height / 2.0f));
	sprite->setPosition(sf::Vector2f(body->GetPosition().x, body->GetPosition().y));
	sprite->setRotation(body->GetAngle()*180.0f / 3.14159f);
}