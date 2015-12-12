#include "Client.h"

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
	/*if (mpTimer->shouldUpdate())
	{
		if (getFirstConnected())
		{
			sendPaddleData(mGameInfo.lPlayer.x, mGameInfo.lPlayer.y);
		}
		else
		{
			sendPaddleData(mGameInfo.rPlayer.x, mGameInfo.rPlayer.y);
		}
	}*/
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
			GameInfo gameInfo = *reinterpret_cast<GameInfo*>(mpPacket->data);
			break;
		}

		default:
			//default handle packets in GameStructs
			handlePackets(mPacketIdentifier, mpPacket);
			break;
		}
	}
}

/*
void Client::sendPaddleData(float x, float y)
{
	Position pos;
	pos.x = x;
	pos.y = y;
	pos.mID = ID_SEND_PADDLE_DATA;
	mpClient->Send((const char*)&pos, sizeof(pos), HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}*/