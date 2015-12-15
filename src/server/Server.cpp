#include "Server.h"
#include "../common/PhysicsStructs.h"

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
}


void Server::cleanup()
{
	//shutdown server
	mpServer->Shutdown(300);
	//We're done with the network
	RakNet::RakPeerInterface::DestroyInstance(mpServer);
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

	//update GameInfos
	updateGames();

	//if enough time has passed (30fps), broadcast game states to clients
	/*if (mpTimer->shouldUpdate())
	{
		broadcastGameInfo();
	}*/
}


void Server::updateGames()
{
	for (unsigned int i = 0; i < mGameInfos.size(); i++)
	{
		//if the game has started
		if (mGameInfos[i].started)
		{
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
			int j = 0;
			for (unsigned int i = 0; i < mGames.size(); i++)
			{
				GameInfo gameInfo = *reinterpret_cast<GameInfo*>(p->data);

				gameInfo.mID = ID_RECIEVE_GAME_INFO;

				if (mGames[i][0] == p->guid)
				{
					mpServer->Send((const char*)&gameInfo, sizeof(gameInfo), HIGH_PRIORITY, RELIABLE_ORDERED, 0, mGames[i][1], false);
				}
				else if (mGames[i][1] == p->guid)
				{
					mpServer->Send((const char*)&gameInfo, sizeof(gameInfo), HIGH_PRIORITY, RELIABLE_ORDERED, 0, mGames[i][0], false);
				}

				if (i % 2 == 1) j++;
			}

			break;
		}
/*
		//the client sends over time to start game
		case ID_START_GAME:
		{
			//find correct game and client
			int j = 0;
			for (unsigned int i = 0; i < mClientPairs.size(); i++)
			{
				if (mGuidPairs[i][0] == p->guid || mGuidPairs[i][1] == p->guid)
				{
					resetGame(j);
				}
				if (i % 2 == 1) j++;
			}

			break;
		}*/
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

	mGameInfos[index].firstPlayer.position  = position(0, 0);
	mGameInfos[index].secondPlayer.position = position(0, 0);

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

		//increase number of games
		mCurrentNumberOfGames++;
	}
}