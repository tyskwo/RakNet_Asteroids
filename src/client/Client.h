#ifndef CLIENT_H
#define CLIENT_H

//library includes
#include <array>

//RakNet includes
#include "Gets.h"

//game includes
#include "../common/NetworkStructs.h"

class Client
{
public:
	Client();
	Client(const char* clientPort, const char* serverAddress, const char* serverPort);
	~Client();

	void init(const char* clientPort, const char* serverAddress, const char* serverPort);
	void cleanup();

	void update();



	//get/set flags for whether or not it was the first connected player
	inline void setFirstConnected(bool wasFirst) { mWasFirstConnected = wasFirst; };
	inline bool getFirstConnected()			     { return mWasFirstConnected;     };

	//get/set flags for whether or not it is connected
	inline void setConnected(bool isConnected)   { mIsConnected = isConnected; };
	inline bool getConnected()					 { return mIsConnected;        };

	//getter for the corresponding game info from server
	inline void setGameInfo(GameInfo info) { mGameInfo = info; };
	inline GameInfo getGameInfo() { return mGameInfo; };

private:
	//pointer to client object
	RakNet::RakPeerInterface* mpClient;

	//Holds packets
	RakNet::Packet* mpPacket;

	//socket descriptor
	RakNet::SocketDescriptor mSocketDescriptor;

	RakNet::RakNetGUID mServerGuid;

	//get and identify packet
	unsigned char mPacketIdentifier;
	unsigned char GetPacketIdentifier(RakNet::Packet* pPacket);
	void getPackets();

	void sendShipData();

	//flag for if client is connected, and if so, first player
	bool mIsConnected;
	bool mWasFirstConnected;

	//game info
	GameInfo mGameInfo;

	RakNet::Time mLastUpdateSent;
};

#endif