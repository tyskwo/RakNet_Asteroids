#include "PhysicsStructs.h"
#include "Bullet.h"

void ContactListener::BeginContact(b2Contact* contact)
{
	//get the two bodies
	b2Body* bodyA = contact->GetFixtureA()->GetBody();
	b2Body* bodyB = contact->GetFixtureB()->GetBody();

	//and the two mask filters
	const b2Filter filterA = contact->GetFixtureA()->GetFilterData();
	const b2Filter filterB = contact->GetFixtureB()->GetFilterData();


	if (filterA.categoryBits == PHYSICS::category::FIRST_BULLET)
	{
		//remove bullet
		Bullet* bullet = (Bullet*)(bodyA->GetUserData());
		bullet->setDelete(true);

		if (filterB.categoryBits == PHYSICS::category::SECOND_SHIP)
		{
			//lower second ship's life count
		}
		if (filterB.categoryBits == PHYSICS::category::ASTEROID)
		{
			//split asteroid
		}
	}
	else if (filterB.categoryBits == PHYSICS::category::FIRST_BULLET)
	{
		//remove bullet
		Bullet* bullet = (Bullet*)(bodyA->GetUserData());
		bullet->setDelete(true);

		if (filterA.categoryBits == PHYSICS::category::SECOND_SHIP)
		{
			//lower second ship's life count
		}
		if (filterA.categoryBits == PHYSICS::category::ASTEROID)
		{
			//split asteroid
		}
	}

	if (filterA.categoryBits == PHYSICS::category::SECOND_BULLET)
	{
		//remove bullet
		Bullet* bullet = (Bullet*)(bodyA->GetUserData());
		bullet->setDelete(true);

		if (filterB.categoryBits == PHYSICS::category::FIRST_SHIP)
		{
			//lower first ship's life count
		}
		if (filterB.categoryBits == PHYSICS::category::ASTEROID)
		{
			//split asteroid
		}
	}
	else if (filterB.categoryBits == PHYSICS::category::SECOND_BULLET)
	{
		//remove bullet
		Bullet* bullet = (Bullet*)(bodyA->GetUserData());
		bullet->setDelete(true);

		if (filterA.categoryBits == PHYSICS::category::FIRST_SHIP)
		{
			//lower first ship's life count
		}
		if (filterA.categoryBits == PHYSICS::category::ASTEROID)
		{
			//split asteroid
		}
	}

	if (filterA.categoryBits == PHYSICS::category::ASTEROID)
	{
		if (filterB.categoryBits == PHYSICS::category::FIRST_SHIP)
		{
			//lower first ship's life count
		}
		if (filterB.categoryBits == PHYSICS::category::SECOND_SHIP)
		{
			//lower second ship's life count
		}
	}
	else if (filterB.categoryBits == PHYSICS::category::ASTEROID)
	{
		if (filterA.categoryBits == PHYSICS::category::FIRST_SHIP)
		{
			//lower first ship's life count
		}
		if (filterA.categoryBits == PHYSICS::category::SECOND_SHIP)
		{
			//lower second ship's life count
		}
	}
}