#include "Server.h"

int main(int argc, char** argv)
{
	Server* mpServer;

	if (argc == 2) { mpServer = new Server(argv[1]); } //create server with input port number
	else		   { mpServer = new Server("200"); }   //create server with port number of 200
	
	//run the program as long as the server is running
	while (true) { mpServer->update(); }

	return 0;
}