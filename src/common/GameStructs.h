//file for common structs and values sent between server and client

//constants
const float SCREEN_WIDTH  = 1200.0;
const float SCREEN_HEIGHT = 768.0;
const double PI = std::atan(1.0) * 4;


//struct for position values
struct position
{
	float x, y;
};

//struct for velocity values
struct velocity
{
	float x, y, rot;
};

//struct for player values
struct Player
{
	position pos;
	velocity vel;
	int numLives;
};

//struct for ball values
struct Asteroid
{
	position pos;
	velocity vel;
	bool collidedWith;
};

//struct to receive game state from server
#pragma pack(push, 1)
struct GameInfo
{
	unsigned char mID;
	bool started = false;
	bool finished = false;

	int numLevel = 0;
	int score = 0;

	Player firstPlayer;
	Player secondPlayer;
	Asteroid asteroids;
};
#pragma pack(pop)