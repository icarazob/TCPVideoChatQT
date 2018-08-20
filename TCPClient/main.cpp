#include "TCPClient.h"
#include <iostream>




int main()
{
	TCPClient client{ 5000,"127.0.0.1" };

	if (client.Connect())
	{
		std::cout << "Connect" << std::endl;
		client.RecieveMessage();
	}


	return 0;
}