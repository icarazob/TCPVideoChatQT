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

	m_addr.sin_addr.s_addr = ADDR_ANY;
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

			//Receive packet of the client
			PacketType packet;
			RecievePacket(newClient, packet);

			if (packet == P_ChatMessage)
			{
				//Receive name of the client
				std::string name;
				bool result = RecieveMessage(newClient, name);

				if (result)
				{
					//Insert a new client
					int id = 0;
					bool insertStatus = DB::GetInstance().InserClientInfo(name, id);

					if (insertStatus)
					{
						//Send connected
						SendInformationMessage(newClient, "Connected");
						std::cout << "Client connected!" << std::endl;

						//Receive setup message
						RecievePacket(newClient, packet);
						if (packet == P_InformationMessage)
						{
							if (!ProcessInformationMessage(newClient))
							{
								continue;
							}
						}

						//Add new client to container
						{
							std::lock_guard<std::mutex> lock(m_mutex);
							m_clients.push_back(newClient);
							m_names.push_back(name);
						}

						SendClientsList();

						//Create thread for new client
						std::thread td(CreateHandler(), m_clients.size() - 1);

						//detach thread
						td.detach();
					}
					else
					{
						SendInformationMessage(newClient, "Client with the same name exist");
					}
				}
			}

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

cv::Mat TCPServer::RecieveFrame(SOCKET client)
{
	cv::Mat recieveFrame = cv::Mat::zeros(391,751,CV_8UC3);
	char* uptr = (char*)recieveFrame.data;

	int frameSize;

	//recieve size
	int resultInt = recv(client, (char*)&frameSize, sizeof(int), NULL);

	if (resultInt == SOCKET_ERROR)
	{
		std::cerr << "Client disconnect " << GetLastError() << std::endl;
		return cv::Mat::zeros(cv::Size(0, 0), CV_8UC1);
	}
	//recieve char
	int resultFrame = recv(client, uptr, frameSize, MSG_WAITALL);

	if (resultFrame == SOCKET_ERROR)
	{
		std::cerr << "Error: recieve frame!" << std::endl;
		return cv::Mat::zeros(cv::Size(0, 0), CV_8UC1);
	}

	return recieveFrame;
}

void TCPServer::SendFrame(SOCKET client,cv::Mat frame)
{
	PacketType packet = P_FrameMessage;

	int resultPacket = send(client, (char*)&packet, sizeof(packet), NULL);
	if (resultPacket == SOCKET_ERROR)
	{
		std::cerr << "Don't send a packet message" << GetLastError() << std::endl;
		return;
	}

	frame = (frame.reshape(0, 1));
	int imgSize = frame.total() * frame.elemSize();

	int resultInt = send(client, (char*)&imgSize, sizeof(int), NULL);

	if (resultInt == SOCKET_ERROR)
	{
		std::cerr << "Don't send int message for client" << GetLastError() << std::endl;
		return;
	}

	int result = send(client, (char*)frame.data, imgSize, NULL);

	if (result == SOCKET_ERROR)
	{
		std::cerr << "Don't send message for client" << GetLastError() << std::endl;
		return;
	}

}

bool TCPServer::ReceiveAudio(SOCKET client,char **buffer,int &length)
{
	int bufferSize = 0;

	int resultInt = recv(client, (char*)&bufferSize, sizeof(int), NULL);
	if (resultInt == SOCKET_ERROR)
	{
		std::cerr << "Can't receive size of buffer" << GetLastError() << std::endl;
		return false;
	}

	char *tempBuffer = new char[bufferSize+1];
	tempBuffer[bufferSize] = '\0';

	int resultBuffer = recv(client, tempBuffer, bufferSize, NULL);
	if (resultBuffer == SOCKET_ERROR)
	{
		std::cerr << "Can't receive bytes of audio" << GetLastError() << std::endl;
		return false;
	}

	*buffer = tempBuffer;
	length = bufferSize;

	return true;
}

void TCPServer::SendAudio(SOCKET client,char * buffer, int length)
{

	PacketType packet = P_AudioMessage;

	int resultPacket = send(client, (char*)&packet, sizeof(packet), NULL);
	if (resultPacket == SOCKET_ERROR)
	{
		std::cerr << "Audio: can't send a packet " << GetLastError() << std::endl;
		return;
	}

	int size = length;
	int resultInt = send(client, (char*)&size, sizeof(int), NULL);
	if (resultInt == SOCKET_ERROR)
	{
		std::cerr << "Audio: can't send a buffer size " << GetLastError() << std::endl;
		return;
	}

	int resultBuffer = send(client, buffer, size, NULL);
	if (resultBuffer == SOCKET_ERROR)
	{
		std::cerr << "Audio: can't send a buffer" << GetLastError() << std::endl;
		return;
	}

	

	return;
	

	
}

bool TCPServer::ProcessInformationMessage(SOCKET client)
{
	std::string message;
	bool resultMessage = RecieveMessage(client, message);

	if (!resultMessage)
	{
		return false;
	}

	if (message.compare("Stop Video") == 0)
	{
		std::cout << "Send stop video" << std::endl;
		SendAllWithoutClientInformationMessage(client, "Stop Video");
		return true;
	}
	else if (message.compare("Setup") == 0)
	{
		return true;
	}
	else if (message.compare("Save History") == 0)
	{
		SaveMessageHistoryOfClient(client);
	}
	else if (message.compare("History") == 0)
	{
		std::string clientName;
		PacketType packet;

		if (ProcessPacket(client, packet))
		{
			if (packet == P_ChatMessage)
			{
				RecieveMessage(client, clientName);

				//Get History from database

				//
			}

		}


	}

	return false;
}

void TCPServer::SendInformationMessage(SOCKET client, std::string message)
{
	PacketType packet = P_InformationMessage;

	int resultPacket = send(client, (char*)&packet, sizeof(packet), NULL);
	if (resultPacket == SOCKET_ERROR)
	{
		std::cerr << "Information: can't send a packet" << GetLastError() << std::endl;
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

void TCPServer::SendAllWithoutClientInformationMessage(SOCKET client, std::string message)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	for (int i = 0; i < m_clients.size(); i++)
	{
		if (m_clients[i] == client)
		{
			continue;
		}

		SendInformationMessage(m_clients[i], message);
	}
}

void TCPServer::SendAllInformationMessage(std::string message)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	for (int i = 0; i < m_clients.size(); i++)
	{
		SendInformationMessage(m_clients[i], message);
	}
}

void TCPServer::RecievePacket(SOCKET client,PacketType & packet)
{
	int resultPacket = recv(client, (char*)&packet, sizeof(packet), NULL);

	if (resultPacket == SOCKET_ERROR)
	{
		std::cout << "RecievePacket: error to receive packet" << GetLastError() << std::endl;
	}

	return;
}

void TCPServer::SendClientsList()
{
	SendAllInformationMessage("List");

	std::string strList;
	auto listOfClients = DB::GetInstance().SelectNameOfAllClients();

	for (auto &clientName : listOfClients)
	{
		strList.append(clientName);
		strList.append(" ");
	}

	SendAllInformationMessage(strList);
}

void TCPServer::SaveMessageHistoryOfClient(SOCKET client)
{
	PacketType packet;
	RecievePacket(client, packet);
	std::string clientName;

	if (packet == P_ChatMessage && RecieveMessage(client, clientName))
	{
		RecievePacket(client, packet);

		//Receive ten messages
		std::string clientMessages;
		if (packet == P_ChatMessage && RecieveMessage(client, clientMessages))
		{
			//Find id of client 
			int clientID = DB::GetInstance().GetClientIDByName(clientName);
			
			if (clientID != -1)
			{
				std::cout << clientID << std::endl;
			}

		}
	}

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

	cv::Mat frame;
	std::string message;
	char *audio;
	int length = 0;

	switch (packet)
	{
	case P_ChatMessage:
	{
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
	}
	case P_FrameMessage:
	{
		frame = RecieveFrame(client);

		{
			std::lock_guard<std::mutex> lock(m_mutex);

			for (auto i = 0; i < m_clients.size(); i++)
			{
				if (m_clients[i] == client)
				{
					continue;
				}

				SendFrame(m_clients[i], frame);
			}
		}
		break;
	}
	case P_AudioMessage:
	{
		if (ReceiveAudio(client, &audio, length))
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			for (int i = 0; i < m_clients.size(); i++)
			{
				if (m_clients[i] == client)
				{
					continue;
				}

				SendAudio(m_clients[i], audio, length);

			}

			delete[]audio;
		}

		break;
	}

	case P_InformationMessage:
	{
		if (!ProcessInformationMessage(client))
		{
			return false;
		}
		break;
	}

	default:
		std::cout << "Undefined packet!" << std::endl;
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
		return false;
	}

	char *buffer = new char[message_size + 1];
	buffer[message_size] = '\0';

	int result = recv(client, buffer, message_size, NULL);

	if (result == SOCKET_ERROR)
	{
		std::cerr << "Client disconnect " << GetLastError() << std::endl;
		return false;
	}
	message = buffer;

	delete[]buffer;

	return true;
}

void TCPServer::DeleteClient(SOCKET client)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	auto clientIt = std::find(m_clients.begin(), m_clients.end(), client);
	int positionInt = clientIt - m_clients.begin();
	auto nameIt = m_names.begin() + positionInt;

	//delete from db
	DB::GetInstance().DeleteClient(m_names[positionInt]);

	//Delete socket
	m_clients.erase(clientIt);

	//Delete name
	m_names.erase(nameIt);


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
				DeleteClient(client);

				SendClientsList();
				return;
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(5));
		}
	};
}
