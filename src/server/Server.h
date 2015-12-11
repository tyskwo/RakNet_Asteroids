#ifndef SERVER_H
#define SERVER_H

#include <assert.h>
#include <iostream>
#include <array>
#include <time.h>

#include "BitStream.h"
#include "Gets.h"
#include "MessageIdentifiers.h"
#include "PacketLogger.h"
#include "RakNetStatistics.h"
#include "RakNetTypes.h"
#include "RakPeerInterface.h"
#include "RakSleep.h"

#include "../common/GameStructs.h"


class Server
{
public:
	Server();
	Server(const char* serverPort);
	~Server();

	void init(const char* serverPort);
	void cleanup();

	void update();
	void broadcastGames();

private:
	//pointer to server object
	RakNet::RakPeerInterface* mpServer;

	//Holds packets
	RakNet::Packet* p;

	void getPackets();
	//determine id of packet
	unsigned char packetIdentifier;
	unsigned char GetPacketIdentifier(RakNet::Packet *p);

	//arrays for the games, and info for said games
	int mNumGames = 8;
	std::array<std::array<RakNet::RakNetGUID, 2>, 8> mGames;
	std::array<GameInfo, 8>							 mGameInfos;

	void initializeGames();
	void updateGames();
	void resetGame(int index);
};

#endif