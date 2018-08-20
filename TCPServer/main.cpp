#pragma once
#include "TCPServer.h"
#include <iostream>



int main()
{
	const int port = 5000;
	std::string ip = "127.0.0.1";

	TCPServer server { port,ip.c_str() };

	if (!server.Run())
	{
		std::cerr << "Can't accept a new client!" << std::endl;
		return 1;
	}
	

	
	return 0;
}