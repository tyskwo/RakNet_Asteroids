#ifndef GAMESTRUCTS_H
#define GAMESTRUCTS_H

//so there is no redefinition errors
#pragma once

//Box2D includes
#include <Box2D\Box2D.h>

//SFML includes
#include <SFML/Graphics.hpp>

//library includes
#include <cstdio>

//RakNet includes
#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"


//#####################################################################################################################
//#  FILE FOR COMMON STRUCTS, VALUES, AND METHODS BETWEEN THE SERVER AND CLIENT  ######################################
//#####################################################################################################################



//constants
const float SCREEN_WIDTH  = 1200.0;
const float SCREEN_HEIGHT = 768.0;
const float PI = std::atan(1.0f) * 4.0f;



//struct for textures
struct TEXTURES
{
	static sf::Texture mFirstShip,   mSecondShip,
					   mFirstBullet, mSecondBullet;

	static void init(std::string mBuildType)
	{
		mFirstShip.loadFromFile(mBuildType + "redShip.png");
		mSecondShip.loadFromFile(mBuildType + "blueShip.png");

		mFirstBullet.loadFromFile(mBuildType + "redBullet.png");
		mSecondBullet.loadFromFile(mBuildType + "blueBullet.png");
	}
};



//struct for physics values
struct PHYSICS
{
	enum category 
	{
		FIRST_SHIP    = 0x0001,
		SECOND_SHIP   = 0x0002,
		ASTEROID      = 0x0004,
		FIRST_BULLET  = 0x0008,
		SECOND_BULLET = 0x0010,
	};

	//physics world
	struct WORLD
	{
		static const b2Vec2  gravity()	{ return b2Vec2(0.0f, 0.0f); };
		static const float32 timeStep()	{ return 1.0f / 60.0f; };

		static const int32   velocityIterations = 6;
		static const int32   positionIterations = 2;
	};
	
	struct SHIP
	{
		static const float32 LinearDamping()  { return 0.025f; };
		static const float32 AngularDamping() { return 0.1f;   };
		static const float32 linearSpeed()	  { return 0.1f;   };
		static const float32 rotateSpeed()	  { return 0.008f; };
		static const float32 density()		  { return 1.0f;   };
		static const float32 friction()		  { return 0.3f;   };
	};

	struct BULLET
	{
		static const float32 linearSpeed()		{ return 10.0f;   };
		static const float32 density()			{ return 1.0f;   };
		static const float32 friction()			{ return 0.3f;   };
	};
};

struct OBJECT
{
	b2Body* body = NULL;
	sf::Sprite sprite;
};

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
struct Bullet
{
	position position;
	velocity velocity;
};

//struct for player values
struct Player
{
	position position;
	velocity velocity;
	int numLives;

	std::array<Bullet, 32> bullets;
};

//struct for asteroid values
struct Asteroid
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

	Player firstPlayer;
	Player secondPlayer;
	std::array<Asteroid, 64> asteroids;
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

#endif GAMESTRUCTS_H