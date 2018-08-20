#include "TCPClient.h"

void TCPClient::InitializeWSA()
{
	WORD version = MAKEWORD(2, 1);
	if (WSAStartup(version, &wsaData) != 0)
	{
		std::cerr << "WSA error " << GetLastError() << std::endl;
		exit(1);
	}
}

void TCPClient::CreateSocket()
{
	connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connection == SOCKET_ERROR)
	{
		std::cout << "Can't create a socket" << GetLastError() << std::endl;
		exit(1);
	}

}

TCPClient::TCPClient(int port, const char * ip)
{
	this->port = port;
	this->ip = ip;
	
	InitializeWSA();

	addr.sin_addr.s_addr = inet_addr(ip);
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;

	CreateSocket();

}

bool TCPClient::Connect()
{
	if (connect(connection, (SOCKADDR*)&addr, sizeof(addr)) == 0)
	{
		return true;
	}


	return false;

}

void TCPClient::RecieveMessage()
{
	char buffer[4096];

	int result = recv(connection, buffer, sizeof(buffer), NULL);
	if (result == SOCKET_ERROR)
	{
		std::cerr << "Error: recv message!" << std::endl;
		exit(1);
	}

	std::cout << buffer << std::endl;
	return;
}
