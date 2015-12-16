#ifndef SERVER_H
#define SERVER_H

//library includes
#include <array>
#include <cmath>
#include <Box2D\Box2D.h>
#include "..\common\Trackable.h"

//game includes
#include "../common/NetworkStructs.h"
#include "../common/Game.h"

class Timer;

class Server : public Trackable
{
public:
	Server();
	Server(const char* serverPort);
	~Server();

	void init(const char* serverPort);
	void cleanup();

	void update();
	void broadcastGames();

	std::array<Game*, 8> mpGames;

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
	//spawn asteroid
	void spawnAsteroid();
	void sendAsteroids();

	//set the connection with a client
	void setConnection(RakNet::Packet* p);

	Timer* mpTimer;

	RakNet::Time mLastUpdateSent;

	int mAsteroidIncr;
};

#endif