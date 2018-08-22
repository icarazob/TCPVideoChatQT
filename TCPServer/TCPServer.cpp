#include "TCPServer.h"
#include <thread>
#include <chrono>
#include <string>
#include <string.h>
#include <opencv2/opencv.hpp>



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
			std::thread td(CreateHandler(), m_clients.size() - 1);

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

void TCPServer::RecieveFrameAndShow(SOCKET client)
{
	cv::Mat recieveFrame = cv::Mat::zeros(361,441,CV_8UC3);
	char* uptr = (char*)recieveFrame.data;

	int frameSize;

	//recieve size
	int resultInt = recv(client, (char*)&frameSize, sizeof(int), NULL);

	if (resultInt == SOCKET_ERROR)
	{
		std::cerr << "Client disconnect " << GetLastError() << std::endl;
		DeleteClient(client);
		return;
	}
	//recieve char
	int resultFrame = recv(client, uptr, frameSize, MSG_WAITALL);

	if (resultFrame == SOCKET_ERROR)
	{
		std::cerr << "Error: recieve frame!" << std::endl;
		return;
	}

	//show
	cv::imshow("Video", recieveFrame);

	cv::waitKey(25);

	return;
}

void TCPServer::ShowFrame(char * frame)
{



}

TCPServer::~TCPServer()
{
	
}



bool TCPServer::ProcessPacket(SOCKET client,PacketType & packet)
{
	int resultPacket = recv(client, (char*)&packet, sizeof(packet), NULL);

	if (resultPacket == SOCKET_ERROR)
	{
		std::cerr << "Error: process packet!" << GetLastError() << std::endl;
		return false;
	}

	std::string message;
	switch (packet)
	{
	case P_ChatMessage:
		RecieveMessage(client, message);

		{
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

		break;
	case P_FrameMessage:
		RecieveFrameAndShow(client);
		break;
	default:
		return false;
	}

	return true;
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
	PacketType packet = P_ChatMessage;

	int resultPacket = send(client, (char*)&packet, sizeof(packet), NULL);
	if (resultPacket == SOCKET_ERROR)
	{
		std::cerr << "Don't send a packet message" << GetLastError() << std::endl;
		return;
	}

	int messageSize = message.size();
	int resultInt = send(client, (char*)&messageSize, sizeof(int), NULL);

	if (resultInt == SOCKET_ERROR)
	{
		std::cerr << "Don't send int message for client" << GetLastError() << std::endl;
		return;
	}

	int result = send(client, message.c_str(), messageSize, NULL);

	if (result == SOCKET_ERROR)
	{
		std::cerr << "Don't send message for client" << GetLastError() << std::endl;
		return;
	}
}

bool TCPServer::RecieveMessage(SOCKET client, std::string & message)
{
	int message_size = 0;

	int resultInt = recv(client, (char*)&message_size, sizeof(int), NULL);

	if (resultInt == SOCKET_ERROR)
	{
		std::cerr << "Client disconnect " << GetLastError() << std::endl;
		DeleteClient(client);
		return false;
	}

	char *buffer = new char[message_size + 1];
	buffer[message_size] = '\0';

	int result = recv(client, buffer, message_size, NULL);

	message = buffer;

	delete[]buffer;
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
		SOCKET client = 0;
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			client = m_clients[clientNumber];
		}


		while (true)
		{
			PacketType packet;

			if (!ProcessPacket(client, packet))
			{
				std::cout << "Close Thread!" << std::endl;
				return;
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(5));
		}
	};
}
