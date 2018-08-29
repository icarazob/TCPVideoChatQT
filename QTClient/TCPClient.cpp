#include "TCPClient.h"
#include <thread>
#include <chrono>
#include <QMessageBox>
#include <QDebug>


bool TCPClient::ProcessPacket(PacketType & packet)
{
	int resultPacket = recv(m_connection, (char*)&packet, sizeof(packet), NULL);

	if (resultPacket == SOCKET_ERROR)
	{
		return false;
	}

	switch (packet)
	{
	case P_AudioMessage:
	{
		RecieveAudio();
		break;
	}
	case P_ChatMessage:
	{
		RecieveMessage();
		break;
	}

	case P_FrameMessage:
	{
		RecieveFrame();
		break;
	}
	case P_InformationMessage:
	{
		RecieveInformationMessage();
		break;
	}


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

TCPClient::~TCPClient()
{

}

bool TCPClient::Connect()
{
	int result = ::connect(m_connection, (SOCKADDR*)&m_addr, sizeof(m_addr));
	if ( result == 0)
	{

		//Create thread
		std::thread td(CreateProcessHandler());

		//detach thread
		td.detach();

		return true;
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

	Q_EMIT recieveEventMessage(buffer);

	delete[]buffer;
	//Append Message


	//std::cout << buffer << std::endl;
	
	return;
}
void TCPClient::RecieveInformationMessage()
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

	std::string stringMessage = buffer;

	if (stringMessage.compare("Stop Video") == 0)
	{
		Q_EMIT clearLabel();
	}


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

void TCPClient::SendAudio(QByteArray buffer, int length)
{
	while (!m_proceed)
	{

	}

	m_proceed = false;
	std::lock_guard<std::mutex> lock(m_mutex);

	PacketType packet = P_AudioMessage;

	int resutlPacket = send(m_connection, (char*)&packet, sizeof(packet), NULL);
	if (resutlPacket == SOCKET_ERROR)
	{
		return;
	}


	int resultInt = send(m_connection, (char*)&length, sizeof(int), NULL);
	if (resultInt == SOCKET_ERROR)
	{
		return;
	}


	int resultData = send(m_connection,buffer.data(), length, NULL);
	if (resultData == SOCKET_ERROR)
	{
		return;
	}

	m_proceed = true;

}

void TCPClient::SendInformationMessage(std::string message)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	PacketType packet = P_InformationMessage;

	int resultPacket = send(m_connection, (char*)&packet, sizeof(packet), NULL);
	if (resultPacket == SOCKET_ERROR)
	{
		return;
	}

	int messageSize = message.size();

	int resultInt = send(m_connection, (char*)&messageSize, sizeof(int), NULL);

	if (resultInt == SOCKET_ERROR)
	{
		std::cerr << "Can't send message" << GetLastError() << std::endl;
		return;
	}

	int result = send(m_connection, message.c_str(), messageSize, NULL);

	if (result == SOCKET_ERROR)
	{
		std::cerr << "Can't send message" << GetLastError() << std::endl;
		return;
	}

	return;
}



cv::Mat TCPClient::GetCurrentFrame()
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_currentFrame;
}

void TCPClient::RecieveFrame()
{
	cv::Mat recieveFrame = cv::Mat::zeros(391, 751, CV_8UC3);
	char* uptr = (char*)recieveFrame.data;

	int frameSize;
	int resultInt = recv(m_connection, (char*)&frameSize, sizeof(int), NULL);

	if (resultInt == SOCKET_ERROR)
	{
		return;
	}

	int resultFrame = recv(m_connection, uptr, frameSize, MSG_WAITALL);

	if (resultFrame == SOCKET_ERROR)
	{
		return;
	}

	m_currentFrame = recieveFrame.clone();
	Q_EMIT recieveEventFrame();


	return;

}

void TCPClient::RecieveAudio()
{
	int length;

	int resultInt = recv(m_connection, (char*)&length, sizeof(int), NULL);
	if (resultInt == SOCKET_ERROR)
	{
		qDebug() << "Audio: don't receive length";
		return;
	}

	char *buffer = new char[length+1];
	buffer[length] = '\0';

	int resultData = recv(m_connection, buffer, length, NULL);
	if (resultData == SOCKET_ERROR)
	{
		qDebug() << "Audio: don't receive buffer" << resultData;
		return;
	}

	QByteArray	qBuffer = QByteArray(buffer, length);
	Q_EMIT recieveEventAudio(qBuffer, length);

	delete[]buffer;

	return;
}