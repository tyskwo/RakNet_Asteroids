#ifndef SERVER_H
#define SERVER_H

//library includes
#include <array>
#include <cmath>

//game includes
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
	int mCurrentNumberOfGames = 0;
	std::array<std::array<RakNet::RakNetGUID, 2>, 8> mGames;
	std::array<GameInfo, 8>	mGameInfos;

	//initialize game infos
	void initializeGames();
	//update game infos
	void updateGames();
	//reset given game info
	void resetGame(int index);

	//set the connection with a client
	void setConnection(RakNet::Packet* p);
};

#endif