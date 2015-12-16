#include "Client.h"
#include "../common/Ship.h"
#include "../common/Bullet.h"
#include "GetTime.h"

Client::Client()
{
	//value for the IP.
	char mIPaddress[64];

	//get ip through command prompt
	puts("Enter IP to connect to:");
	char temp[32];
	Gets(temp, sizeof(temp));
	Gets(mIPaddress, sizeof(mIPaddress));

	//if garbage is entered, set to localhost
	if (mIPaddress[0] == 0) strcpy_s(mIPaddress, "127.0.0.1");

	//init with client port of 202 and entered ip address
	init("202", mIPaddress, "200");

	mServerGuid = RakNet::UNASSIGNED_RAKNET_GUID;
}

Client::Client(const char* clientPort, const char* serverAddress, const char* serverPort)
{
	init(clientPort, serverAddress, serverPort);
}

Client::~Client()
{
	cleanup();
}

void Client::init(const char* clientPort, const char* serverAddress, const char* serverPort)
{
	//not connected yet
	mIsConnected = false;

	//create client instance
	mpClient = RakNet::RakPeerInterface::GetInstance();
	mpClient->AllowConnectionResponseIPMigration(false);


	//set up connection the client to the server
	mSocketDescriptor = RakNet::SocketDescriptor(atoi(clientPort), 0);
	mSocketDescriptor.socketFamily = AF_INET;
	mpClient->Startup(8, &mSocketDescriptor, 1);
	mpClient->SetOccasionalPing(true);

	//try to connect
	RakNet::ConnectionAttemptResult car = mpClient->Connect(serverAddress, atoi(serverPort), "hello", (int)strlen("hello"));
	RakAssert(car == RakNet::CONNECTION_ATTEMPT_STARTED);

	//mShipData.firstPlayer.velocity = velocity(0.0f, 0.0f, 0.0f);
	//mShipData.secondPlayer.velocity = velocity(0.0f, 0.0f, 0.0f);

	mLastUpdateSent = float(RakNet::GetTimeMS());
}

void Client::cleanup()
{
	//shut down the client
	mpClient->Shutdown(300);
	//and end the connection
	RakNet::RakPeerInterface::DestroyInstance(mpClient);
}

unsigned char Client::GetPacketIdentifier(RakNet::Packet *p)
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

void Client::update()
{
	// Get a packet from either the server or the client
	getPackets();

	//if enough time has passed (30fps), broadcast game states to clients
	if (float(RakNet::GetTimeMS()) - mLastUpdateSent > 50.0f)
	{
		mGameInfo.timeStamp = RakNet::GetTime();

		while (mBulletData.size() > 0)
		{
			mpClient->Send((const char*)&mBulletData.front(), sizeof(mBulletData.front()), HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, mServerGuid, false);
			mBulletData.pop();
		}

		sendShipData();
		mLastUpdateSent = float(RakNet::GetTimeMS());
	}
}

void Client::getPackets()
{
	// Get a packet from either the server or the client
	for (mpPacket = mpClient->Receive(); mpPacket; mpClient->DeallocatePacket(mpPacket), mpPacket = mpClient->Receive())
	{
		// We got a packet, get the identifier with our handy function
		mPacketIdentifier = GetPacketIdentifier(mpPacket);

		// Check if this is a network message packet
		switch (mPacketIdentifier)
		{
		case ID_FIRST_CONNECTION:
		{
			mServerGuid = mpPacket->guid;

			//set as first connected or second connected.
			setFirstConnected(true);
			setConnected(true);

			//get the packet's GameInfo struct and set it equal to ours
			GameInfo gameInfo = *reinterpret_cast<GameInfo*>(mpPacket->data);
			mGameInfo = gameInfo;

			printf("%d", gameInfo.gameNumber);
			break;
		}
		case ID_SECOND_CONNECTION:
		{
			mServerGuid = mpPacket->guid;

			//set as first connected or second connected.
			setFirstConnected(false);
			setConnected(true);

			//get the packet's GameInfo struct and set it equal to ours
			GameInfo gameInfo = *reinterpret_cast<GameInfo*>(mpPacket->data);
			mGameInfo = gameInfo;

			printf("%d", gameInfo.gameNumber);
			break;
		}

		case ID_RECIEVE_GAME_INFO:
		{
			//get the packet's GameInfo struct
			BothShips shipData = *reinterpret_cast<BothShips*>(mpPacket->data);
			mShipData = shipData;

			if (mShipStates.size() == 0)
			{
				mShipStates.push(shipData);
				mTwoPacketsAgo = mOnePacketAgo;
				mOnePacketAgo = mShipData.timeStamp;
			}
			else if(mShipData.timeStamp > mShipStates.back().timeStamp)
			{
				mShipStates.push(shipData);
				mTwoPacketsAgo = mOnePacketAgo;
				mOnePacketAgo = mShipData.timeStamp;
			}
			

			mJustRecieved = true;
			break;
		}

		case ID_RECIEVE_BULLET_INFO:
		{
			//get the packet's GameInfo struct
			BulletData bulletData = *reinterpret_cast<BulletData*>(mpPacket->data);
			printf("%d\n", bulletData.index);
			if (mWasFirstConnected)
			{
				mpGame->getSecondPlayerBullets()[bulletData.index] = new Bullet(false, mpGame->getSecondPlayer(), mpGame->getWorld());
				/*b2Vec2 position(bulletData.bullet.position.x, bulletData.bullet.position.y);
				mpGame->getSecondPlayerBullets()[bulletData.index]->getBody()->SetTransform(position, bulletData.bullet.position.angle);

				b2Vec2 velocity(bulletData.bullet.velocity.x, bulletData.bullet.velocity.y);
				mpGame->getSecondPlayerBullets()[bulletData.index]->getBody()->SetLinearVelocity(velocity);
				mpGame->getSecondPlayerBullets()[bulletData.index]->getBody()->SetAngularVelocity(bulletData.bullet.velocity.rot);*/
			}
			else
			{
				mpGame->getFirstPlayerBullets()[bulletData.index] = new Bullet(true, mpGame->getFirstPlayer(), mpGame->getWorld());
				/*b2Vec2 position(bulletData.bullet.position.x, bulletData.bullet.position.y);
				mpGame->getFirstPlayerBullets()[bulletData.index]->getBody()->SetTransform(position, bulletData.bullet.position.angle);

				b2Vec2 velocity(bulletData.bullet.velocity.x, bulletData.bullet.velocity.y);
				mpGame->getFirstPlayerBullets()[bulletData.index]->getBody()->SetLinearVelocity(velocity);
				mpGame->getFirstPlayerBullets()[bulletData.index]->getBody()->SetAngularVelocity(bulletData.bullet.velocity.rot);*/
			}

			break;
		}

		default:
			//default handle packets in GameStructs
			handlePackets(mPacketIdentifier, mpPacket);
			break;
		}
	}
}


void Client::sendShipData()
{
	mShipData.mID = ID_SEND_SHIP_INFO;
	mShipData.timeStamp = RakNet::GetTime();
	mpClient->Send((const char*)&mShipData, sizeof(mShipData), HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, mServerGuid, false);
}

BothShips Client::getBestState()
{
	while (mShipStates.size() > 1 && float(RakNet::GetTime() - mShipStates.front().timeStamp) > delay)
	{
		mShipStates.pop();
	}

	return mShipStates.front();
}