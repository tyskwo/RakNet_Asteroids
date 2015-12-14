#include "Asteroid.h"

Asteroid::Asteroid() { init(); };

Asteroid::Asteroid(const Asteroid &other)
{
	body = other.body;
	sprite = other.sprite;
}

Asteroid::~Asteroid() {};

void Asteroid::init()
{
	initSize();
	initPhysics();
	initSprite();
}

void Asteroid::initSize() { size = static_cast<Size>(rand() % 3); }

void Asteroid::initPhysics()
{
	b2CircleShape circleShape;
	if (size == SMALL)       { circleShape.m_radius = 25;  }
	else if (size == MEDIUM) { circleShape.m_radius = 50;  }
	else if (size == LARGE)  { circleShape.m_radius = 100; }

	b2Vec2 position;
	position.x = SCREEN_WIDTH / 2.0f;
	position.y = SCREEN_HEIGHT / 2.0f;
	float32 angle = (static_cast <float32> (rand()) / static_cast <float32> (RAND_MAX)) * PI * 2;

	b2BodyDef bodyDefinition;
	bodyDefinition.type = b2_dynamicBody;
	bodyDefinition.position.Set(position.x, position.y);

	body = physicsWorld->CreateBody(&bodyDefinition);
	body->SetTransform(position, angle);


	static b2FixtureDef fix;
	fix.shape = &circleShape;
	fix.density = mPhysics::density();
	fix.filter.categoryBits = PHYSICS::category::ASTEROID;
	fix.filter.maskBits     = PHYSICS::category::FIRST_SHIP    | 
							  PHYSICS::category::SECOND_SHIP   | 
							  PHYSICS::category::FIRST_BULLET  | 
							  PHYSICS::category::SECOND_BULLET | 
							  PHYSICS::category::ASTEROID;
	body->CreateFixture(&fix);


	body->SetLinearVelocity(b2Vec2(mPhysics::linearSpeed()*cos(body->GetAngle()),
		                           mPhysics::linearSpeed()*sin(body->GetAngle())));
}

void Asteroid::initSprite()
{
	sprite = new sf::Sprite();
	int randomType = rand() % 3;

	if (randomType == 0)
	{
		if (size == SMALL)       { sprite->setTexture(TEXTURES::mSmallAsteroid1);  }
		else if (size == MEDIUM) { sprite->setTexture(TEXTURES::mMediumAsteroid1); }
		else if (size == LARGE)  { sprite->setTexture(TEXTURES::mLargeAsteroid1);  }
	}
	else if (randomType == 1)
	{
		if (size == SMALL)       { sprite->setTexture(TEXTURES::mSmallAsteroid2);  }
		else if (size == MEDIUM) { sprite->setTexture(TEXTURES::mMediumAsteroid2); }
		else if (size == LARGE)  { sprite->setTexture(TEXTURES::mLargeAsteroid2);  }
	}
	else if (randomType == 2)
	{
		if (size == SMALL)       { sprite->setTexture(TEXTURES::mSmallAsteroid3);  }
		else if (size == MEDIUM) { sprite->setTexture(TEXTURES::mMediumAsteroid3); }
		else if (size == LARGE)  { sprite->setTexture(TEXTURES::mLargeAsteroid3);  }
	}

	//set sprite's properties
	sprite->setOrigin(sf::Vector2f(sprite->getLocalBounds().width / 2.0f, sprite->getLocalBounds().height / 2.0f));
	sprite->setPosition(sf::Vector2f(body->GetPosition().x, body->GetPosition().y));
	sprite->setRotation(body->GetAngle()*180.0f / 3.14159f);
}