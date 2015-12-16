#include "Client.h"

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
	mOtherShipInterpolation = ObjectInfoBuffer();

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
	if ((RakNet::GetTime() - mLastUpdateSent) > 50.0f)
	{
		mGameInfo.timeStamp = RakNet::GetTime();
		sendShipData();
		mLastUpdateSent = (RakNet::GetTime() - mLastUpdateSent);
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

			ObjectState shipState;
			!mWasFirstConnected ? shipState.mAngle = shipData.firstPlayer.position.angle : shipState.mAngle = shipData.secondPlayer.position.angle;
			!mWasFirstConnected ? shipState.mVelAngular = shipData.firstPlayer.velocity.rot : shipState.mVelAngular = shipData.secondPlayer.velocity.rot;
			!mWasFirstConnected ? shipState.mVelX = shipData.firstPlayer.velocity.x : shipState.mVelX = shipData.secondPlayer.velocity.x;
			!mWasFirstConnected ? shipState.mVelY = shipData.firstPlayer.velocity.y : shipState.mVelY = shipData.secondPlayer.velocity.y;
			!mWasFirstConnected ? shipState.mX = shipData.firstPlayer.position.x : shipState.mX = shipData.secondPlayer.position.x;
			!mWasFirstConnected ? shipState.mY = shipData.firstPlayer.position.y : shipState.mY = shipData.secondPlayer.position.y;

			ObjectInfo shipInfo;
			shipInfo.SetState(shipState);
			mOtherShipInterpolation.SetStartingInfo(shipInfo);

			mJustRecieved = true;
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
	mpClient->Send((const char*)&mShipData, sizeof(mShipData), HIGH_PRIORITY, RELIABLE_ORDERED, 0, mServerGuid, false);
}


void Client::setShipPosAndVel(float x, float y, float angle, float xVel, float yVel, float angularVel)
{
	mWasFirstConnected ? mShipData.secondPlayer.position.angle = angle : mShipData.firstPlayer.position.angle = angle;
	mWasFirstConnected ? mShipData.secondPlayer.position.x = x : mShipData.firstPlayer.position.x = x;
	mWasFirstConnected ? mShipData.secondPlayer.position.y = y : mShipData.firstPlayer.position.y = y;
	mWasFirstConnected ? mShipData.secondPlayer.velocity.x = xVel : mShipData.firstPlayer.velocity.x = xVel;
	mWasFirstConnected ? mShipData.secondPlayer.velocity.y = yVel : mShipData.firstPlayer.velocity.y = yVel;
	mWasFirstConnected ? mShipData.secondPlayer.velocity.rot = angularVel : mShipData.firstPlayer.velocity.rot = angularVel;
}