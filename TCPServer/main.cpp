#pragma once
#include "TCPServer.h"
#include <iostream>
#include <thread>







int main()
{
	const int port = 5000;
	std::string ip = "127.0.0.1";

	TCPServer server { port,ip.c_str() };

	server.Run();

	
	return 0;
}