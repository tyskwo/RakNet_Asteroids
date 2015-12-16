#include "PhysicsStructs.h"
#include "Bullet.h"
#include "Ship.h"
#include "Asteroid.h"

void ServerContactListener::BeginContact(b2Contact* contact)
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
			Ship* ship = (Ship*)(bodyB->GetUserData());
			ship->loseHealth(1);
		}
		if (filterB.categoryBits == PHYSICS::category::ASTEROID)
		{
			//split asteroid
			Asteroid* asteroid = (Asteroid*)(bodyB->GetUserData());
			asteroid->loseHealth(1);
		}
	}
	else if (filterB.categoryBits == PHYSICS::category::FIRST_BULLET)
	{
		//remove bullet
		Bullet* bullet = (Bullet*)(bodyB->GetUserData());
		bullet->setDelete(true);

		if (filterA.categoryBits == PHYSICS::category::SECOND_SHIP)
		{
			//lower second ship's life count
			Ship* ship = (Ship*)(bodyA->GetUserData());
			ship->loseHealth(1);
		}
		if (filterA.categoryBits == PHYSICS::category::ASTEROID)
		{
			//split asteroid
			Asteroid* asteroid = (Asteroid*)(bodyA->GetUserData());
			asteroid->loseHealth(1);
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
			Ship* ship = (Ship*)(bodyB->GetUserData());
			ship->loseHealth(1);
		}
		if (filterB.categoryBits == PHYSICS::category::ASTEROID)
		{
			//split asteroid
			Asteroid* asteroid = (Asteroid*)(bodyB->GetUserData());
			asteroid->loseHealth(1);
		}
	}
	else if (filterB.categoryBits == PHYSICS::category::SECOND_BULLET)
	{
		//remove bullet
		Bullet* bullet = (Bullet*)(bodyB->GetUserData());
		bullet->setDelete(true);

		if (filterA.categoryBits == PHYSICS::category::FIRST_SHIP)
		{
			//lower first ship's life count
			Ship* ship = (Ship*)(bodyA->GetUserData());
			ship->loseHealth(1);
		}
		if (filterA.categoryBits == PHYSICS::category::ASTEROID)
		{
			//split asteroid
			Asteroid* asteroid = (Asteroid*)(bodyA->GetUserData());
			asteroid->loseHealth(1);
		}
	}





	if (filterA.categoryBits == PHYSICS::category::ASTEROID)
	{
		Asteroid* asteroid = (Asteroid*)(bodyA->GetUserData());

		if (filterB.categoryBits == PHYSICS::category::FIRST_SHIP || filterB.categoryBits == PHYSICS::category::SECOND_SHIP)
		{
			Ship* ship = (Ship*)(bodyB->GetUserData());
			ship->loseHealth(1 + asteroid->getSize());
		}
	}
	else if (filterB.categoryBits == PHYSICS::category::ASTEROID)
	{
		Asteroid* asteroid = (Asteroid*)(bodyB->GetUserData());

		if (filterA.categoryBits == PHYSICS::category::FIRST_SHIP || filterA.categoryBits == PHYSICS::category::SECOND_SHIP)
		{
			Ship* ship = (Ship*)(bodyA->GetUserData());
			ship->loseHealth(1 + asteroid->getSize());
		}
	}
}

void ClientContactListener::BeginContact(b2Contact* contact)
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
			Ship* ship = (Ship*)(bodyB->GetUserData());
			ship->loseHealth(1);
		}
		if (filterB.categoryBits == PHYSICS::category::ASTEROID)
		{
			//split asteroid
			//Asteroid* asteroid = (Asteroid*)(bodyB->GetUserData());
			//asteroid->loseHealth(1);
		}
	}
	else if (filterB.categoryBits == PHYSICS::category::FIRST_BULLET)
	{
		//remove bullet
		Bullet* bullet = (Bullet*)(bodyB->GetUserData());
		bullet->setDelete(true);

		if (filterA.categoryBits == PHYSICS::category::SECOND_SHIP)
		{
			//lower second ship's life count
			Ship* ship = (Ship*)(bodyA->GetUserData());
			ship->loseHealth(1);
		}
		if (filterA.categoryBits == PHYSICS::category::ASTEROID)
		{
			//split asteroid
			//Asteroid* asteroid = (Asteroid*)(bodyA->GetUserData());
			//asteroid->loseHealth(1);
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
			Ship* ship = (Ship*)(bodyB->GetUserData());
			ship->loseHealth(1);
		}
		if (filterB.categoryBits == PHYSICS::category::ASTEROID)
		{
			//split asteroid
			//Asteroid* asteroid = (Asteroid*)(bodyB->GetUserData());
			//asteroid->loseHealth(1);
		}
	}
	else if (filterB.categoryBits == PHYSICS::category::SECOND_BULLET)
	{
		//remove bullet
		Bullet* bullet = (Bullet*)(bodyB->GetUserData());
		bullet->setDelete(true);

		if (filterA.categoryBits == PHYSICS::category::FIRST_SHIP)
		{
			//lower first ship's life count
			Ship* ship = (Ship*)(bodyA->GetUserData());
			ship->loseHealth(1);
		}
		if (filterA.categoryBits == PHYSICS::category::ASTEROID)
		{
			//split asteroid
			//Asteroid* asteroid = (Asteroid*)(bodyA->GetUserData());
			//asteroid->loseHealth(1);
		}
	}





	if (filterA.categoryBits == PHYSICS::category::ASTEROID)
	{
		Asteroid* asteroid = (Asteroid*)(bodyA->GetUserData());

		if (filterB.categoryBits == PHYSICS::category::FIRST_SHIP || filterB.categoryBits == PHYSICS::category::SECOND_SHIP)
		{
			Ship* ship = (Ship*)(bodyB->GetUserData());
			ship->loseHealth(1 + asteroid->getSize());
		}
	}
	else if (filterB.categoryBits == PHYSICS::category::ASTEROID)
	{
		Asteroid* asteroid = (Asteroid*)(bodyB->GetUserData());

		if (filterA.categoryBits == PHYSICS::category::FIRST_SHIP || filterA.categoryBits == PHYSICS::category::SECOND_SHIP)
		{
			Ship* ship = (Ship*)(bodyA->GetUserData());
			ship->loseHealth(1 + asteroid->getSize());
		}
	}
}