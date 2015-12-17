#include "Asteroid.h"

#include "..\common\PhysicsStructs.h"

Asteroid::Asteroid() { init(); };

Asteroid::Asteroid(b2World* pWorld) :Object(pWorld) { init(); };

Asteroid::Asteroid(int size, b2Vec2 position, b2Vec2 velocity, float rotVel, float angle, int type, b2World* pWorld)
:Object(pWorld)
{
	initAlreadyCreated(size, position, velocity, rotVel, angle, type);
}

void Asteroid::initAlreadyCreated(int size, b2Vec2 position, b2Vec2 velocity, float rotVel, float angle, int type)
{
	this->size = static_cast<Size>(size);
	health = (size + 1) * (size + 1);

	initPhysics(position);

	body->SetLinearVelocity(velocity);
	body->SetAngularVelocity(rotVel);
	body->SetTransform(position, angle);

	initSprite(type);
}

Asteroid::Asteroid(int size, b2Vec2 location, b2World* pWorld)
:Object(pWorld)
{  
	this->size = static_cast<Size>(size);
	health = (size + 1) * (size + 1);

	initPhysics(location);
	initSprite();
}

Asteroid::Asteroid(const Asteroid &other, b2World* pWorld)
:Object(pWorld)
{
	body = other.body;
	sprite = other.sprite;
}

Asteroid::~Asteroid() {};

void Asteroid::init()
{
	initSize();

	b2Vec2 position;
	//position.x = SCREEN_WIDTH / 2.0f;
	//position.y = SCREEN_HEIGHT / 2.0f;
	int xRand = rand() % 2, yRand = rand() % 2;
	xRand == 0 ? position.x = static_cast<float>(rand() % 150 - 300) : position.x = static_cast<float>(SCREEN_WIDTH + (rand() % 150 + 150));
	yRand == 0 ? position.y = static_cast<float>(rand() % 150 - 300) : position.y = static_cast<float>(SCREEN_HEIGHT + (rand() % 150 + 150));
	initPhysics(position);
	initSprite(rand() % 3);
}

void Asteroid::initSize() 
{ 
	size = static_cast<Size>(rand() % 3); 
	health = (size + 1) * (size + 1);
}

void Asteroid::initPhysics(b2Vec2 location)
{
	b2CircleShape circleShape;
	if (size == SMALL)       { circleShape.m_radius = 25;  }
	else if (size == MEDIUM) { circleShape.m_radius = 50;  }
	else if (size == LARGE)  { circleShape.m_radius = 100; }

	float32 angle = (static_cast <float32> (rand()) / static_cast <float32> (RAND_MAX)) * PI * 2;

	b2BodyDef bodyDefinition;
	bodyDefinition.type = b2_kinematicBody;
	bodyDefinition.position.Set(location.x, location.y);

	body = mpWorld->CreateBody(&bodyDefinition);
	body->SetTransform(location, angle);


	static b2FixtureDef fix;
	fix.shape = &circleShape;
	fix.density = mPhysics::density();
	fix.filter.categoryBits = PHYSICS::category::ASTEROID;
	fix.filter.maskBits = PHYSICS::category::FIRST_SHIP |
		PHYSICS::category::SECOND_SHIP |
		PHYSICS::category::FIRST_BULLET |
		PHYSICS::category::SECOND_BULLET;
							  //PHYSICS::category::ASTEROID;
	body->CreateFixture(&fix);


	body->SetLinearVelocity(b2Vec2(mPhysics::linearSpeed()*(3-size)*cos(body->GetAngle()) * 2.5f,
		                           mPhysics::linearSpeed()*(3-size)*sin(body->GetAngle()) * 2.5f));

	body->SetUserData(this);
}

void Asteroid::initSprite(int type)
{
	sprite = new sf::Sprite();
	int randomType = type;

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

void Asteroid::loseHealth(int damage)
{
	health -= damage;

	if (collisionSpawn == NoSpawn)
	{
		if (size == LARGE)
		{
			if (health <= 6)
			{
				//break up into three mediums and delete self
				collisionSpawn.numberToSpawn = 3;
				collisionSpawn.sizeToSpawn   = 1;
				collisionSpawn.deleteSelf    = true;
			}
			else
			{
				//spawn off one small
				collisionSpawn.numberToSpawn = 1;
				collisionSpawn.sizeToSpawn   = 0;
				collisionSpawn.deleteSelf    = false;
			}
		}
		else if (size == MEDIUM)
		{
			if (health <= 2)
			{
				//break up into two smalls and delete self
				collisionSpawn.numberToSpawn = 2;
				collisionSpawn.sizeToSpawn   = 0;
				collisionSpawn.deleteSelf    = true;
			}
			else
			{
				//spawn off one small
				collisionSpawn.numberToSpawn = 1;
				collisionSpawn.sizeToSpawn   = 0;
				collisionSpawn.deleteSelf    = false;
			}
		}
		else
		{
			//delete self
			collisionSpawn.numberToSpawn = 0;
			collisionSpawn.sizeToSpawn   = 0;
			collisionSpawn.deleteSelf    = true;
		}
	}
}

bool Asteroid::isOnScreen()
{
	bool onScreen = true;

	if (body->GetPosition().x < -sprite->getLocalBounds().width)  { onScreen = false; }
	if (body->GetPosition().y < -sprite->getLocalBounds().height) { onScreen = false; }
	if (body->GetPosition().x > SCREEN_WIDTH  + sprite->getLocalBounds().width)  { onScreen = false; }
	if (body->GetPosition().y > SCREEN_HEIGHT + sprite->getLocalBounds().height) { onScreen = false; }

	return onScreen;
}


void Asteroid::interpolate(RakNet::Time currentTime, RakNet::Time onePacketAgo, RakNet::Time twoPacketAgo)
{
	float time = (float(currentTime) - startTime) / (targetTime - startTime);
	if (time > 1.0f) time = 1.0f;

	if (abs(targetState.data.position.x - startState.data.position.x) > SCREEN_WIDTH ||
		abs(targetState.data.position.y - startState.data.position.y) > SCREEN_HEIGHT)
	{
		b2Vec2 position(targetState.data.position.x, targetState.data.position.y);
		body->SetTransform(position, targetState.data.position.angle);
		sprite->setPosition(targetState.data.position.x, targetState.data.position.y);
		b2Vec2 velocity(0, 0);
		body->SetLinearVelocity(velocity);
		body->SetAngularVelocity(0);
		time = 1.0f;
	}

	else
	{
		float32 currentX = startState.data.position.x * (1.0f - time) + targetState.data.position.x * time;
		float32 currentY = startState.data.position.y * (1.0f - time) + targetState.data.position.y * time;
		float32 currentAngle = startState.data.position.angle * (1.0f - time) + targetState.data.position.angle * time;
		b2Vec2 position(currentX, currentY);

		body->SetTransform(position, currentAngle);
		sprite->setPosition(currentX, currentY);

		float32 currentXVelocity = startState.data.velocity.x * (1.0f - time) + targetState.data.velocity.x * time;
		float32 currentYVelocity = startState.data.velocity.y * (1.0f - time) + targetState.data.velocity.y * time;
		float32 currentAngleVelocity = startState.data.velocity.rot * (1.0f - time) + targetState.data.velocity.rot * time;
		b2Vec2 velocity(0, 0);

		body->SetLinearVelocity(velocity);
		body->SetAngularVelocity(0);
	}
	if (time >= 1.0f) { isDoneInterpolating = true; }

}
void Asteroid::addInterpolation(AsteroidObject data)
{
	targetState.data = data;

	AsteroidObject currentAsteroid;
	currentAsteroid.position = position(body->GetPosition().x, body->GetPosition().y, body->GetAngle());
	currentAsteroid.velocity = velocity(body->GetLinearVelocity().x, body->GetLinearVelocity().y, body->GetAngularVelocity());

	startState.data = currentAsteroid;

	startTime = float(RakNet::GetTime());
	targetTime = float(RakNet::GetTime()) + 100.0f;
}

position Asteroid::getPosition()
{
	position pos;

	pos.x     = body->GetPosition().x;
	pos.y     = body->GetPosition().y;
	pos.angle = body->GetAngle();

	return pos;
}

void Asteroid::cleanup()
{
	mShouldDelete = true;

	if (body != NULL)
	{
		mpWorld->DestroyBody(body);
	}

	delete(sprite);
	sprite = NULL;
}