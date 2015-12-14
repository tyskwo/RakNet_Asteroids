#ifndef NETWORKSTRUCTS_H
#define NETWORKSTRUCTS_H

//so there is no redefinition errors
#pragma once

//library includes
#include <cstdio>
#include <array>

//RakNet includes
#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"


//#####################################################################################################################
//#  FILE FOR NETWORK STRUCTS, VALUES, AND METHODS BETWEEN THE SERVER AND CLIENT  #####################################
//#####################################################################################################################

//struct for position values
struct position
{
	float x, y;

	position()
	{
		x = 0;
		y = 0;
	}
	position(float x, float y)
	{
		this->x = x;
		this->y = y;
	}
};

//struct for velocity values
struct velocity
{
	float x, y, rot;

	velocity() 
	{
		x = 0;
		y = 0;
		rot = 0;
	}
	velocity(float x, float y, float rot)
	{
		this->x   = x;
		this->y   = y;
		this->rot = rot;
	}
};

//struct for bullet values
struct BulletObject
{
	position position;
	velocity velocity;
};

//struct for player values
struct ShipObject
{
	position position;
	velocity velocity;
	int numLives;

	std::array<BulletObject, 16> bullets;
};

//struct for asteroid values
struct AsteroidObject
{
	unsigned char size;
	position position;
	velocity velocity;
};

//struct to receive game state from server
#pragma pack(push, 1)
struct GameInfo
{
	//1 byte
	unsigned char mID;

	int  gameNumber;

	//2 bytes
	bool started  = false;
	bool finished = false;

	//3 bytes
	char numLevel = 0;
	unsigned short score = 0;

	//6 bytes so far.

	ShipObject firstPlayer;
	ShipObject secondPlayer;
	std::array<AsteroidObject, 64> asteroids;
};
#pragma pack(pop)

//enum for the packet IDs
enum MessageTypes
{
	ID_SEND_PADDLE_DATA = ID_USER_PACKET_ENUM,
	ID_FIRST_CONNECTION,
	ID_SECOND_CONNECTION,
	ID_RECIEVE_GAME_INFO,
};


//method for handling built-in packet IDs
static void handlePackets(unsigned char mPacketIdentifier, RakNet::Packet* mpPacket)
{
	switch (mPacketIdentifier)
	{
	case ID_DISCONNECTION_NOTIFICATION:
		// Connection lost normally
		printf("ID_DISCONNECTION_NOTIFICATION\n");
		break;
	case ID_ALREADY_CONNECTED:
		printf("ID_ALREADY_CONNECTED with guid %" PRINTF_64_BIT_MODIFIER "u\n", mpPacket->guid);
		break;
	case ID_INCOMPATIBLE_PROTOCOL_VERSION:
		printf("ID_INCOMPATIBLE_PROTOCOL_VERSION\n");
		break;
	case ID_REMOTE_DISCONNECTION_NOTIFICATION: // Server telling the clients of another client disconnecting gracefully.  You can manually broadcast this in a peer to peer enviroment if you want.
		printf("ID_REMOTE_DISCONNECTION_NOTIFICATION\n");
		break;
	case ID_REMOTE_CONNECTION_LOST: // Server telling the clients of another client disconnecting forcefully.  You can manually broadcast this in a peer to peer enviroment if you want.
		printf("ID_REMOTE_CONNECTION_LOST\n");
		break;
	case ID_REMOTE_NEW_INCOMING_CONNECTION: // Server telling the clients of another client connecting.  You can manually broadcast this in a peer to peer enviroment if you want.
		printf("ID_REMOTE_NEW_INCOMING_CONNECTION\n");
		break;
	case ID_CONNECTION_BANNED: // Banned from this server
		printf("We are banned from this server.\n");
		break;
	case ID_CONNECTION_ATTEMPT_FAILED:
		printf("Connection attempt failed\n");
		break;
	case ID_NO_FREE_INCOMING_CONNECTIONS:
		// Sorry, the server is full.  I don't do anything here but
		// A real app should tell the user
		printf("ID_NO_FREE_INCOMING_CONNECTIONS\n");
		break;
	case ID_INVALID_PASSWORD:
		printf("ID_INVALID_PASSWORD\n");
		break;
	case ID_CONNECTION_LOST:
		// Couldn't deliver a reliable packet - i.e. the other system was abnormally
		// terminated
		printf("ID_CONNECTION_LOST\n");
		break;
	case ID_CONNECTION_REQUEST_ACCEPTED:
		// This tells the client they have connected
		//printf("ID_CONNECTION_REQUEST_ACCEPTED to %s with GUID %s\n", mpPacket->systemAddress.ToString(true), mpPacket->guid.ToString());
		//printf("My external address is %s\n", mpClient->GetExternalID(mpPacket->systemAddress).ToString(true));
		//setConnected(true);
		break;
	case ID_CONNECTED_PING:
		break;
	case ID_UNCONNECTED_PING:
		printf("Ping from %s\n", mpPacket->systemAddress.ToString(true));
		break;

	default:
		//try to print data
		printf("%s\n", mpPacket->data);
		break;
	}
}

#endif 