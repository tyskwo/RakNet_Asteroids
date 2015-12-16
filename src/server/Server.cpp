#include "Server.h"

#include "..\common\PhysicsStructs.h"
#include "..\common\Ship.h"
#include "..\common\Bullet.h"
#include "..\common\Asteroid.h"


#include "GetTime.h"


Server::Server()
{
	//create with port 200
	init("200");
}

Server::Server(const char* serverPort)
{
	init(serverPort);
}

Server::~Server()
{
	cleanup();
}

void Server::init(const char* serverPort)
{
	//get RakNet instance
	mpServer = RakNet::RakPeerInterface::GetInstance();

	//set timeout time of no connection and password for connecting
	mpServer->SetTimeoutTime(30000, RakNet::UNASSIGNED_SYSTEM_ADDRESS);
	mpServer->SetIncomingPassword("hello", (int)strlen("hello"));

	//set up server
	RakNet::SocketDescriptor socketDescriptors[2];
	socketDescriptors[0].port = atoi(serverPort);
	socketDescriptors[0].socketFamily = AF_INET; // Test out IPV4
	socketDescriptors[1].port = atoi(serverPort);
	socketDescriptors[1].socketFamily = AF_INET6; // Test out IPV6

	//start up server and set maximum connections
	bool b = mpServer->Startup(16, socketDescriptors, 1) == RakNet::RAKNET_STARTED;
	mpServer->SetMaximumIncomingConnections(16);

	//server can't start for some reason
	if (!b)
	{
		puts("Server failed to start.  Terminating.");
		exit(1);
	}

	//set flags
	mpServer->SetOccasionalPing(true);
	mpServer->SetUnreliableTimeout(1000);

	//print out ip address
	printf("\nIP address for client to connect:\n");
	for (unsigned int i = 0; i < mpServer->GetNumberOfAddresses(); i++)
	{
		RakNet::SystemAddress sa = mpServer->GetInternalID(RakNet::UNASSIGNED_SYSTEM_ADDRESS, i);
		printf("%s\n", sa.ToString(false));
	}

	//initialize game pairs
	for (unsigned int i = 0; i < mGames.size(); i++)
	{
		mGames[i][0] = RakNet::UNASSIGNED_RAKNET_GUID;
		mGames[i][1] = RakNet::UNASSIGNED_RAKNET_GUID;
	}

	//initialize GameInfos
	initializeGames();
	for (unsigned int i = 0; i < mpGames.size(); i++) { mpGames[i] = new Game(true); }

	mAsteroidIncr = 0;
}


void Server::cleanup()
{
	//shutdown server
	mpServer->Shutdown(300);
	//We're done with the network
	RakNet::RakPeerInterface::DestroyInstance(mpServer);

	for (unsigned int i = 0; i < mpGames.size(); i++)
	{
		if (mpGames[i] != NULL)
		{
			delete mpGames[i];
			mpGames[i] = NULL;
		}
	}
}

unsigned char Server::GetPacketIdentifier(RakNet::Packet *p)
{
	if (p == 0)
		return 255;

	if ((unsigned char)p->data[0] == ID_TIMESTAMP)
	{
		RakAssert(p->length > sizeof(RakNet::MessageID) + sizeof(RakNet::Time));
		return (unsigned char)p->data[sizeof(RakNet::MessageID) + sizeof(RakNet::Time)];
	}
	else
		return (unsigned char)p->data[0];
}

void Server::update()
{
	//get packets from clients
	getPackets();
	//updateGames();
	mAsteroidIncr++;

	if ((RakNet::GetTime() - mLastUpdateSent) > 50.0f)
	{
		sendAsteroids();
		mLastUpdateSent = (RakNet::GetTime() - mLastUpdateSent);
		
		if (mAsteroidIncr > 500)
		{
			spawnAsteroid();
			mAsteroidIncr = 0;
		}
	}
}

void Server::updateGames()
{
	for (unsigned int i = 0; i < mpGames.size(); i++)
	{
		mpGames[i]->update();
	}
}

void Server::spawnAsteroid()
{
	for (unsigned int i = 0; i < mpGames.size(); i++)
	{
		if (mpGames[i] != NULL && mpGames[i]->getHasStarted())
		{
			int index = mpGames[i]->spawnAsteroid();

			if (index != -1)
			{
				AsteroidData asteroid;
				asteroid.asteroid.index = index;
				asteroid.asteroid.position = position(mpGames[i]->getAsteroids()[index]->getBody()->GetPosition().x, mpGames[i]->getAsteroids()[index]->getBody()->GetPosition().y, mpGames[i]->getAsteroids()[index]->getBody()->GetAngle());
				asteroid.asteroid.velocity = velocity(mpGames[i]->getAsteroids()[index]->getBody()->GetLinearVelocity().x, mpGames[i]->getAsteroids()[index]->getBody()->GetLinearVelocity().y, mpGames[i]->getAsteroids()[index]->getBody()->GetAngularVelocity());
				asteroid.asteroid.health = mpGames[i]->getAsteroids()[index]->getHealth();
				asteroid.asteroid.size = mpGames[i]->getAsteroids()[index]->getSize();
				asteroid.asteroid.type = rand() % 3;

				asteroid.mID = ID_RECIEVE_NEW_ASTEROID;
				mpServer->Send((const char*)&asteroid, sizeof(asteroid), HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, mGames[i][0], false);
				mpServer->Send((const char*)&asteroid, sizeof(asteroid), HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, mGames[i][1], false);
			}
		}
	}
}

void Server::getPackets()
{
	for (p = mpServer->Receive(); p; mpServer->DeallocatePacket(p), p = mpServer->Receive())
	{
		// We got a packet, get the identifier with our handy function
		packetIdentifier = GetPacketIdentifier(p);

		// Check if this is a network message packet
		switch (packetIdentifier)
		{
		case ID_NEW_INCOMING_CONNECTION:
		{
			//add client to Games list.
			setConnection(p);
			break;
		}
		case ID_SEND_SHIP_INFO:
		{
			//find correct game and client
			for (unsigned int i = 0; i < mGames.size(); i++)
			{
				BothShips shipData = *reinterpret_cast<BothShips*>(p->data);

				shipData.mID = ID_RECIEVE_GAME_INFO;
				shipData.timeStamp = RakNet::GetTime();

				if (mGames[i][0] == p->guid)
				{
					b2Vec2 position(shipData.firstPlayer.position.x, shipData.firstPlayer.position.y);
					mpGames[i]->getFirstPlayer()->getBody()->SetTransform(position, shipData.firstPlayer.position.angle);

					b2Vec2 velocity(shipData.firstPlayer.velocity.x, shipData.firstPlayer.velocity.y);
					mpGames[i]->getFirstPlayer()->getBody()->SetLinearVelocity(velocity);
					mpGames[i]->getFirstPlayer()->getBody()->SetAngularVelocity(shipData.firstPlayer.velocity.rot);


					mpServer->Send((const char*)&shipData, sizeof(shipData), HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, mGames[i][1], false);
				}
				else if (mGames[i][1] == p->guid)
				{
					b2Vec2 position(shipData.secondPlayer.position.x, shipData.secondPlayer.position.y);
					mpGames[i]->getSecondPlayer()->getBody()->SetTransform(position, shipData.secondPlayer.position.angle);

					b2Vec2 velocity(shipData.secondPlayer.velocity.x, shipData.secondPlayer.velocity.y);
					mpGames[i]->getSecondPlayer()->getBody()->SetLinearVelocity(velocity);
					mpGames[i]->getSecondPlayer()->getBody()->SetAngularVelocity(shipData.secondPlayer.velocity.rot);

					mpServer->Send((const char*)&shipData, sizeof(shipData), HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, mGames[i][0], false);
				}
			}

			break;
		}

		case ID_SEND_BULLET_INFO:
		{
			//find correct game and client
			for (unsigned int i = 0; i < mGames.size(); i++)
			{
				BulletData bulletData = *reinterpret_cast<BulletData*>(p->data);
				//bulletData.bullet.position.x += bulletData.bullet.velocity.x * (RakNet::GetTime() - bulletData.timeStamp);
				//bulletData.bullet.position.y += bulletData.bullet.velocity.y * (RakNet::GetTime() - bulletData.timeStamp);

				bulletData.mID = ID_RECIEVE_BULLET_INFO;

				if (mGames[i][0] == p->guid)
				{
					//mpGames[i]->getFirstPlayerBullets()[bulletData.index]->getBody()->SetLinearVelocity(;
					mpServer->Send((const char*)&bulletData, sizeof(bulletData), HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, mGames[i][1], false);
				}
				else if (mGames[i][1] == p->guid)
				{
					//mpGames[i]->fireBullet(false);
					mpServer->Send((const char*)&bulletData, sizeof(bulletData), HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, mGames[i][0], false);
				}
			}

			break;
		}
		default:
			//default handle packets in GameStructs
			handlePackets(packetIdentifier, p);
			break;
		}
	}
}

void Server::broadcastGames()
{
	//send each GameInfo to the correct clients
	for (unsigned int i = 0; i < mGames.size(); i++)
	{
		GameInfo info = mGameInfos[i];
		info.mID = ID_RECIEVE_GAME_INFO;
		mpServer->Send((const char*)&info, sizeof(info), HIGH_PRIORITY, RELIABLE_ORDERED, 0, mGames[i][0], false);
		mpServer->Send((const char*)&info, sizeof(info), HIGH_PRIORITY, RELIABLE_ORDERED, 0, mGames[i][1], false);
	}
}

void Server::initializeGames()
{
	//reset games to init values
	for (unsigned int i = 0; i < mGames.size(); i++) { resetGame(i); }
}

void Server::resetGame(int index)
{
	mGameInfos[index].mID = ID_RECIEVE_GAME_INFO;

	//mGameInfos[index].firstPlayer.numLives  = 3;
	//mGameInfos[index].secondPlayer.numLives = 3;

	mGameInfos[index].firstPlayer.velocity  = velocity(0, 0, 0);
	mGameInfos[index].secondPlayer.velocity = velocity(0, 0, 0);

	mGameInfos[index].firstPlayer.position  = position(0, 0, 0);
	mGameInfos[index].secondPlayer.position = position(0, 0, 0);

	mGameInfos[index].gameNumber = index;
	mGameInfos[index].finished   = false;
	mGameInfos[index].numLevel   = 1;
	mGameInfos[index].score      = 0;
	mGameInfos[index].started    = false;
}

void Server::setConnection(RakNet::Packet* p)
{
	//Somebody connected.  We have their IP now
	printf("New connection from %s\n", p->systemAddress.ToString(true));

	//if its the first client in a pair
	if (mGames[mCurrentNumberOfGames][0] == RakNet::UNASSIGNED_RAKNET_GUID &&
		mGames[mCurrentNumberOfGames][1] == RakNet::UNASSIGNED_RAKNET_GUID)
	{
		//set guid
		mGames[mCurrentNumberOfGames][0] = p->guid;

		GameInfo info = mGameInfos[mCurrentNumberOfGames];
		info.mID = ID_FIRST_CONNECTION;
		mpServer->Send((const char*)&info, sizeof(info), HIGH_PRIORITY, RELIABLE_ORDERED, 0, p->guid, false);
	}

	//if its the second client in a pair
	else
	{
		mGames[mCurrentNumberOfGames][1] = p->guid;

		GameInfo info = mGameInfos[mCurrentNumberOfGames];
		info.mID = ID_SECOND_CONNECTION;
		mpServer->Send((const char*)&info, sizeof(info), HIGH_PRIORITY, RELIABLE_ORDERED, 0, p->guid, false);

		mGameInfos[mCurrentNumberOfGames].started = true;

		mpGames[mCurrentNumberOfGames]->setHasStarted(true);

		//increase number of games
		mCurrentNumberOfGames++;
	}
}

void Server::sendAsteroids()
{
	for (unsigned int i = 0; i < mGames.size(); i++)
	{
		AsteroidsData asteroids;
		asteroids.mID = ID_RECIEVE_ASTEROID_INFO;
		asteroids.timeStamp = RakNet::GetTime();

		for (unsigned int j = 0; j < mpGames[i]->getAsteroids().size(); j++)
		{
			if (mpGames[i]->getAsteroids()[j] != NULL)
			{
				asteroids.asteroids[j].health = mpGames[i]->getAsteroids()[j]->getHealth();
				asteroids.asteroids[j].position = mpGames[i]->getAsteroids()[j]->getPosition();

				asteroids.asteroids[j].timeStamp = RakNet::GetTime();
			}
			else
			{
				asteroids.asteroids[j].isNULL = true;
			}
		}

		mpServer->Send((const char*)&asteroids, sizeof(asteroids), HIGH_PRIORITY, RELIABLE_ORDERED, 0, mGames[i][0], false);
		mpServer->Send((const char*)&asteroids, sizeof(asteroids), HIGH_PRIORITY, RELIABLE_ORDERED, 0, mGames[i][1], false);
	}

}