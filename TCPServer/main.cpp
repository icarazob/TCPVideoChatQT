#pragma once
#include "TCPServer.h"
#include <iostream>
#include <QApplication>



int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	const int port = 5000;
	std::string ip = "ADDR_ANY";

	TCPServer server { port,ip.c_str() };

	if (!server.Run())
	{
		std::cerr << "Can't accept a new client!" << std::endl;
		return 1;
	}
	

	return app.exec();
}