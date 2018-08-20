#pragma once
#include "TCPClient.h"
#include <iostream>
#include <thread>
#include <chrono>



int main()
{
	TCPClient client{ 5000,"127.0.0.1" };

	if (client.Connect())
	{
		std::string msg;

		while (true)
		{
			std::getline(std::cin, msg);
			client.SendMessage(msg);

			std::this_thread::sleep_for(std::chrono::milliseconds(10));

		}
	}


	return 0;
}