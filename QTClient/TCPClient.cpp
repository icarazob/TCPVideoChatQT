#include "TCPClient.h"
#include <thread>
#include <chrono>
#include <QMessageBox>

bool TCPClient::ProcessPacket(PacketType & packet)
{
	int resultPacket = recv(m_connection, (char*)&packet, sizeof(packet), NULL);

	if (resultPacket == SOCKET_ERROR)
	{
		return false;
	}

	switch (packet)
	{
	case P_ChatMessage:
		RecieveMessage();
		break;
	case P_FrameMessage:

		break;
	default:
		return false;
		break;
	}

	return true;
}

void TCPClient::InitializeWSA()
{
	WORD version = MAKEWORD(2, 1);
	if (WSAStartup(version, &m_wsaData) != 0)
	{
		std::cerr << "WSA error " << GetLastError() << std::endl;
		exit(1);
	}
}

std::function<void (void)> TCPClient::CreateProcessHandler()
{
	return [this]() {

		while (true)
		{
			PacketType packet;

			if (!ProcessPacket(packet))
			{
				return;
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

	};
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

TCPClient::TCPClient(int port, const char * ip,std::string name)
{
	this->m_port = port;
	this->m_ip = ip;
	this->m_name = name;
	
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
	
	while (iteration <= c_maxIteration)
	{
		if (::connect(m_connection, (SOCKADDR*)&m_addr, sizeof(m_addr)) == 0)
		{
			
			//Create thread
			std::thread td(CreateProcessHandler());
			
			//detach thread
			td.detach();

			return true;
		}

		//std::cout << "Client can't connected!" << std::endl;
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

	Q_EMIT recieveEvent(buffer);

	//delete[]buffer;
	//Append Message


	//std::cout << buffer << std::endl;
	
	return;
}
void TCPClient::SendMessage(std::string message)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	PacketType packet = P_ChatMessage;

	int resultPacket = send(m_connection, (char*)&packet, sizeof(packet), NULL);
	if(resultPacket == SOCKET_ERROR)
	{
		std::cerr << "Can't send packet message" << GetLastError() << std::endl;
		return;
	}

	std::string tempName = m_name + ": ";
	std::string mesageWithName = message.insert(0, tempName);
	int messageSize = mesageWithName.size();

	int resultInt = send(m_connection, (char*)&messageSize, sizeof(int), NULL);

	if (resultInt == SOCKET_ERROR)
	{
		std::cerr << "Can't send message" << GetLastError() << std::endl;
		return;
	}

	int result = send(m_connection, mesageWithName.c_str(), messageSize, NULL);

	if (result == SOCKET_ERROR)
	{
		std::cerr << "Can't send message" << GetLastError() << std::endl;
		return;
	}
}

void TCPClient::SendFrame(cv::Mat frame)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	PacketType packet = P_FrameMessage;
	//packet
	int resultPacket = send(m_connection, (char*)&packet, sizeof(packet), NULL);

	if (resultPacket == SOCKET_ERROR)
	{
		return;
	}
	frame = (frame.reshape(0, 1));
	int imgSize = frame.total() * frame.elemSize();

	//send int
	int resultInt = send(m_connection, (char*)&imgSize, sizeof(int), NULL);

	if (resultInt == SOCKET_ERROR)
	{
		return;
	}
	//send frame
	int resultFrame = send(m_connection,(char*)(frame.data), imgSize, NULL);

	if (resultInt == SOCKET_ERROR)
	{
		return;
	}
	
	return;
}
