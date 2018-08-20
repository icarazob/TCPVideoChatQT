#include "TCPClient.h"
#include <thread>
#include <chrono>


void TCPClient::InitializeWSA()
{
	WORD version = MAKEWORD(2, 1);
	if (WSAStartup(version, &m_wsaData) != 0)
	{
		std::cerr << "WSA error " << GetLastError() << std::endl;
		exit(1);
	}
}

void TCPClient::CreateSocket()
{
	m_connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (m_connection == SOCKET_ERROR)
	{
		std::cout << "Can't create a socket" << GetLastError() << std::endl;
		exit(1);
	}

}

TCPClient::TCPClient(int port, const char * ip)
{
	this->m_port = port;
	this->m_ip = ip;
	
	InitializeWSA();

	m_addr.sin_addr.s_addr = inet_addr(ip);
	m_addr.sin_port = htons(port);
	m_addr.sin_family = AF_INET;

	CreateSocket();

}

bool TCPClient::Connect()
{
	const int c_maxIteration = 10;
	int iteration = 0;
	
	while (iteration < c_maxIteration)
	{
		if (connect(m_connection, (SOCKADDR*)&m_addr, sizeof(m_addr)) == 0)
		{
			auto recieveMessageHandler = [this]() {
				int messageSize = 0;

				while (true)
				{
					int resultInt = recv(m_connection, (char*)&messageSize, sizeof(int), NULL);

					if (resultInt == SOCKET_ERROR)
					{
						std::cerr << "Error: recv int " << GetLastError() << std::endl;

						return;
					}

					char *message = new char[messageSize + 1];
					message[messageSize] = '\0';

					int result = recv(m_connection, message, messageSize, NULL);

					if (result != SOCKET_ERROR)
					{
						std::cout << message << std::endl;

						return;
					}

					std::this_thread::sleep_for(std::chrono::milliseconds(10));
				}

			};

			std::thread td(recieveMessageHandler);

			td.detach();

			return true;
		}

		std::cout << "Client can't connected!" << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		++iteration;
	}
	




	return false;

}

void TCPClient::RecieveMessage()
{
	int messageSize = 0;
	int resultInt = recv(m_connection, (char*)&messageSize, sizeof(int), NULL);

	if (resultInt == SOCKET_ERROR)
	{
		std::cerr << "Error: recv message!" << std::endl;
		exit(1);
	}
	char *buffer = new char[messageSize + 1];
	buffer[messageSize] = '\0';

	int result = recv(m_connection, buffer, messageSize, NULL);
	if (result == SOCKET_ERROR)
	{
		std::cerr << "Error: recv message!" << std::endl;
		exit(1);
	}

	std::cout << buffer << std::endl;
	return;
}

void TCPClient::SendMessage(std::string message)
{
	int messageSize = message.size();

	int resultInt = send(m_connection, (char*)&messageSize, sizeof(int), NULL);

	if (resultInt == SOCKET_ERROR)
	{
		std::cerr << "Can't send message" << GetLastError() << std::endl;
		exit(1);
	}

	int result = send(m_connection, message.c_str(), messageSize, NULL);

	if (result == SOCKET_ERROR)
	{
		std::cerr << "Can't send message" << GetLastError() << std::endl;
		exit(1);
	}
}
