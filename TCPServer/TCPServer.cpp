#include "TCPServer.h"
#include <thread>
#include <chrono>
#include <string>




TCPServer::TCPServer(int port, const char * ip)
{
	this->m_ip = ip;
	this->m_port = port;
	InitializeWSA();

	m_addr.sin_addr.s_addr = inet_addr(ip);
	m_addr.sin_port = htons(port);
	m_addr.sin_family = AF_INET;

	CreateBindListenSocket();
}

bool TCPServer::Run()
{
	const std::string c_welcome = "WELCOME!";
	std::cout << "Server start!" << std::endl;
	ShowServerInformation();

	std::function<void (int)> funThread = CreateHandler();
	int sizeOfAddr = sizeof(m_addr);

	while (true)
	{
		SOCKET newClient = 0;
		if (newClient = accept(m_listenSocket, (SOCKADDR*)&m_addr, &sizeOfAddr))
		{
			std::cout << "Client connected!" << std::endl;
			SendMessage(newClient, c_welcome);
			
			//Add new client to container
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				m_clients.push_back(newClient);
			}
			//Create thread for new client
			std::thread td(funThread, m_clients.size() - 1);

			//detach thread
			td.detach();
		}
		else
		{
			return false;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

}

void TCPServer::ShowServerInformation()
{
	std::cout << "Information: " << "\n";
	std::cout << "Ip: " << m_ip << "\n";
	std::cout << "Port: " << m_port << "\n";

	return;
}

TCPServer::~TCPServer()
{
	
}



void TCPServer::InitializeWSA()
{
	WORD version = MAKEWORD(2, 1);

	if (WSAStartup(version, &m_wsaData) != 0)
	{
		std::cerr << "WSAStartup does not word" << GetLastError();
		exit(1);
	}
}

void TCPServer::CreateBindListenSocket()
{
	m_listenSocket = socket(AF_INET, SOCK_STREAM, NULL);

	if (m_listenSocket == SOCKET_ERROR)
	{
		std::cerr << "Can't create a socket" << GetLastError();
		exit(1);
	}

	if (bind(m_listenSocket, (SOCKADDR*)&m_addr, sizeof(m_addr)) == SOCKET_ERROR)
	{
		std::cerr << "Can't bind with addres" << GetLastError();
		exit(1);
	}

	listen(m_listenSocket, SOMAXCONN);

}

void TCPServer::SendMessage(SOCKET client, const std::string message)
{
	int messageSize = message.size();
	int resultInt = send(client, (char*)&messageSize, sizeof(int), NULL);

	if (resultInt == SOCKET_ERROR)
	{
		std::cerr << "Don't send int message for client" << GetLastError() << std::endl;
	}

	int result = send(client, message.c_str(), messageSize, NULL);

	if (result == SOCKET_ERROR)
	{
		std::cerr << "Don't send message for client" << GetLastError() << std::endl;
	}
}

void TCPServer::DeleteClient(SOCKET client)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	auto position = std::find(m_clients.begin(), m_clients.end(), client);
	m_clients.erase(position);

	return;
}

std::function<void (int)> TCPServer::CreateHandler()
{
	return  [this](int clientNumber)
	{
		SOCKET client = m_clients[clientNumber];

		int message_size = 0;
		while (true)
		{
			int resultInt = recv(client, (char*)&message_size, sizeof(int), NULL);

			if (resultInt == SOCKET_ERROR)
			{
				std::cerr << "Client disconnect " << GetLastError() << std::endl;
				DeleteClient(client);
				return;
			}

			char *message = new char[message_size + 1];
			message[message_size] = '\0';

			int result = recv(client, message, message_size, NULL);


			if (result != SOCKET_ERROR)
			{
				std::cout << "Client " << clientNumber << ": " << message << std::endl;

				std::lock_guard<std::mutex> lock(m_mutex);

				for (auto i = 0; i < m_clients.size(); i++)
				{
					if (m_clients[i] == client)
					{
						continue;
					}

					SendMessage(m_clients[i], message);
				}

			}
			delete[]message;
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
		}
	};
}
