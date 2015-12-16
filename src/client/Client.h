#ifndef CLIENT_H
#define CLIENT_H

//library includes
#include <array>
#include <queue>

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

	std::queue<BothShips> mShipStates;


	//get/set flags for whether or not it was the first connected player
	inline void setFirstConnected(bool wasFirst) { mWasFirstConnected = wasFirst; };
	inline bool getFirstConnected()			     { return mWasFirstConnected;     };

	//get/set flags for whether or not it is connected
	inline void setConnected(bool isConnected)   { mIsConnected = isConnected; };
	inline bool getConnected()					 { return mIsConnected;        };

	//getter and setter for the corresponding game info from server
	inline void setGameInfo(GameInfo info) { mGameInfo = info; };
	inline GameInfo getGameInfo() { return mGameInfo; };

	inline void setShipData(BothShips data) { mShipData = data; };
	inline BothShips getShipData() { return mShipData; };

	BothShips getBestState();
	inline bool hasStates() { return mShipStates.size() > 0 ? true : false; };
	RakNet::Time mTwoPacketsAgo = 0;
	RakNet::Time mOnePacketAgo = 0;

	inline void setFireBullet(int index) { mFiredBullet = index; };

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
	void sendBullet(int index);

	int mFiredBullet;

	//flag for if client is connected, and if so, first player
	bool mIsConnected;
	bool mWasFirstConnected;

	//game info
	GameInfo mGameInfo;
	BothShips mShipData;

	float mLastUpdateSent;
	float delay = 20.0f;
};

#endif