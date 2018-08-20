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

void TCPServer::Run()
{
	const std::string c_welcome = "WELCOME!";
	std::cout << "Server start!" << std::endl;
	ShowServerInformation();

	int sizeOfAddr = sizeof(m_addr);
	while (true)
	{
		SOCKET newClient = 0;
		if (newClient = accept(m_listenSocket, (SOCKADDR*)&m_addr, &sizeOfAddr))
		{
			std::cout << "Client connect!" << std::endl;

			SendMessage(newClient, c_welcome);
			/*m_clients.push_back(newClient);*/
			/*std::thread newThread (WorkClient);*/

			
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
}

void TCPServer::ShowServerInformation()
{
	std::cout << "Information: " << "\n";
	std::cout << "Ip: "<< m_ip << "\n";
	std::cout << "Port: " << m_port << "\n";

	return;
}

void TCPServer::WorkClient()
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

void TCPServer::SendMessage(SOCKET client,const std::string message)
{
	int result = send(client, message.c_str(), sizeof(message) + 1, NULL);

	if (result == SOCKET_ERROR)
	{
		std::cerr << "Don't send message for client" << GetLastError() << std::endl;
		exit(1);
	}
}
