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

	std::string message;
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
		RecieveInformationMessage(message);
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
		//Send name
		SendMessageWithoutName(m_name);

		//Receive Information
		PacketType packet;
		
		int resultPacket = recv(m_connection, (char*)&packet, sizeof(packet), NULL);
		if (resultPacket != SOCKET_ERROR)
		{
			std::string message;
			RecieveInformationMessage(message);

			if (message.compare("Client with the same name exist") == 0)
			{
				return false;
			}
			else if (message.compare("Connected") == 0)
			{
				//Create thread
				std::thread td(CreateProcessHandler());

				//detach thread
				td.detach();

				return true;
			}
		}

	}


	return false;

}

void TCPClient::RecieveMessage()
{
	int messageSize = 0;
	int resultInt = recv(m_connection, (char*)&messageSize, sizeof(int), NULL);

	if (resultInt == SOCKET_ERROR)
	{
		qDebug() << "RecieveMessage: error size";
		exit(1);
	}
	char *buffer = new char[messageSize + 1];
	buffer[messageSize] = '\0';

	int result = recv(m_connection, buffer, messageSize, NULL);
	if (result == SOCKET_ERROR)
	{
		qDebug() << "RecieveMessage: error message";
		exit(1);
	}

	Q_EMIT recieveEventMessage(buffer);

	delete[]buffer;
	//Append Message


	//std::cout << buffer << std::endl;
	
	return;
}
void TCPClient::RecieveInformationMessage(std::string &message)
{
	int messageSize = 0;
	int resultInt = recv(m_connection, (char*)&messageSize, sizeof(int), NULL);

	if (resultInt == SOCKET_ERROR)
	{
		qDebug() << "RecieveInformationMessage: error size";
		exit(1);
	}
	char *buffer = new char[messageSize + 1];
	buffer[messageSize] = '\0';

	int result = recv(m_connection, buffer, messageSize, NULL);
	if (result == SOCKET_ERROR)
	{
		qDebug() << "RecieveInformationMessage: error message";
		exit(1);
	}

	std::string stringMessage = buffer;


	if (stringMessage.compare("Stop Video") == 0)
	{
		Q_EMIT clearLabel();
	}
	message = stringMessage;

	delete[]buffer;

	return;

}
void TCPClient::SendMessageWithoutName(std::string message)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	PacketType packet = P_ChatMessage;

	int resultPacket = send(m_connection, (char*)&packet, sizeof(packet), NULL);
	if (resultPacket == SOCKET_ERROR)
	{
		qDebug() << "SendMessageWithoutName: error send packet";
		return;
	}
	int messageSize = message.size();

	int resultInt = send(m_connection, (char*)&messageSize, sizeof(int), NULL);

	if (resultInt == SOCKET_ERROR)
	{
		qDebug() << "SendMessageWithoutName: error send size";
		return;
	}

	int result = send(m_connection, message.c_str(), messageSize, NULL);

	if (result == SOCKET_ERROR)
	{
		qDebug() << "SendMessageWithoutName: error send message";
		return;
	}

	return;
}
void TCPClient::SendMessage(std::string message)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	PacketType packet = P_ChatMessage;

	int resultPacket = send(m_connection, (char*)&packet, sizeof(packet), NULL);
	if(resultPacket == SOCKET_ERROR)
	{
		qDebug() << "SendMessage: error send packet";
		return;
	}

	std::string tempName = m_name + ": ";
	std::string mesageWithName = message.insert(0, tempName);
	int messageSize = mesageWithName.size();

	int resultInt = send(m_connection, (char*)&messageSize, sizeof(int), NULL);

	if (resultInt == SOCKET_ERROR)
	{
		qDebug() << "SendMessage: error send size";
		return;
	}

	int result = send(m_connection, mesageWithName.c_str(), messageSize, NULL);

	if (result == SOCKET_ERROR)
	{
		qDebug() << "SendMessage: error send message";
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
		qDebug() << "SendFrame: error send packet";
		return;
	}
	frame = (frame.reshape(0, 1));
	int imgSize = frame.total() * frame.elemSize();

	//send int
	int resultInt = send(m_connection, (char*)&imgSize, sizeof(int), NULL);

	if (resultInt == SOCKET_ERROR)
	{
		qDebug() << "SendFrame: error send size";
		return;
	}
	//send frame
	int resultFrame = send(m_connection,(char*)(frame.data), imgSize, NULL);

	if (resultInt == SOCKET_ERROR)
	{
		qDebug() << "SendFrame: error send frame";
		return;
	}
	
	return;
}

void TCPClient::SendAudio(QByteArray buffer, int length)
{

	std::lock_guard<std::mutex> lock(m_mutex);

	PacketType packet = P_AudioMessage;

	int resutlPacket = send(m_connection, (char*)&packet, sizeof(packet), NULL);
	if (resutlPacket == SOCKET_ERROR)
	{
		qDebug() << "SendAudio: error send packet";
		return;
	}


	int resultInt = send(m_connection, (char*)&length, sizeof(int), NULL);
	if (resultInt == SOCKET_ERROR)
	{
		qDebug() << "SendAudio: error send size";
		return;
	}


	int resultData = send(m_connection,buffer.data(), length, NULL);
	if (resultData == SOCKET_ERROR)
	{
		qDebug() << "SendAudio: error send audio";
		return;
	}


}

void TCPClient::SendInformationMessage(std::string message)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	PacketType packet = P_InformationMessage;

	int resultPacket = send(m_connection, (char*)&packet, sizeof(packet), NULL);
	if (resultPacket == SOCKET_ERROR)
	{
		qDebug() << "SendInformationMessage: error send packet";
		return;
	}

	int messageSize = message.size();

	int resultInt = send(m_connection, (char*)&messageSize, sizeof(int), NULL);

	if (resultInt == SOCKET_ERROR)
	{
		qDebug() << "SendInformationMessage: error send size";
		return;
	}

	int result = send(m_connection, message.c_str(), messageSize, NULL);

	if (result == SOCKET_ERROR)
	{
		qDebug() << "SendInformationMessage: error send message";
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
		qDebug() << "RecieveFrame: error  size";
		return;
	}

	int resultFrame = recv(m_connection, uptr, frameSize, MSG_WAITALL);

	if (resultFrame == SOCKET_ERROR)
	{
		qDebug() << "RecieveFrame: error  frame";
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
		qDebug() << "RecieveAudio: error  size";
		return;
	}

	char *buffer = new char[length+1];
	buffer[length] = '\0';

	int resultData = recv(m_connection, buffer, length, NULL);
	if (resultData == SOCKET_ERROR)
	{
		qDebug() << "RecieveAudio: error  audio";
		return;
	}

	QByteArray	qBuffer = QByteArray(buffer, length);
	Q_EMIT recieveEventAudio(qBuffer, length);

	delete[]buffer;

	return;
}
