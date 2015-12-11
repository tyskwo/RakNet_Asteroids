#ifndef CLIENT_H
#define CLIENT_H

#include "RakPeerInterface.h"
#include "../common/GameStructs.h"
#include <array>

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
	inline GameInfo getGameInfo() { return mGameInfo; };

private:
	//pointer to client object
	RakNet::RakPeerInterface* mpClient;

	//Holds packets
	RakNet::Packet* mpPacket;

	//socket descriptor
	RakNet::SocketDescriptor mSocketDescriptor;

	//get and identify packet
	unsigned char mPacketIdentifier;
	unsigned char GetPacketIdentifier(RakNet::Packet* pPacket);
	void getPackets();

	//values for the connection values.
	char mIPaddress[64], mServerPort[3], mClientPort[3];

	//flag for if client is connected, and if so, first player
	bool mIsConnected;
	bool mWasFirstConnected;

	//flag to send over game started packet
	bool mSendGameStart;

	//game info
	GameInfo mGameInfo;
};

#endif