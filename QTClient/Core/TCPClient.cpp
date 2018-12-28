#include "TCPClient.h"
#include "InformationStrings.h"
#include "FaceDetector.h"
#include "FaceLandmarkDetector.h"
#include "HaarCascadeDetector.h"
#include "FFmpegLib/H264Decoder.h"
#include <thread>
#include <chrono>
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
	case PacketType::P_AudioMessage:
	{
		RecieveAudio();
		break;
	}
	case PacketType::P_ChatMessage:
	{
		RecieveMessage();
		break;
	}

	case PacketType::P_FrameMessage:
	{
		RecieveFrame();
		break;
	}
	case PacketType::P_InformationMessage:
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

TCPClient::TCPClient(int port, const char * ip, std::string name) :
	m_terminating(false),
	m_port(port),
	m_ip(ip),
	m_name(name),
	m_decoder(std::make_unique<H264Decoder>())
{
	
	InitializeWSA();

	m_addr.sin_addr.s_addr = inet_addr(ip);
	m_addr.sin_port = htons(port);
	m_addr.sin_family = AF_INET;

	CreateSocket();

}

TCPClient::~TCPClient()
{
	if (m_frameThread.joinable())
	{
		m_frameThread.join();
	}
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

			if (message.compare(InformationStrings::IsSameName().toStdString()) == 0)
			{
				m_sameName = true;
				return false;
			}
			else if (message.compare(InformationStrings::Connected().toStdString()) == 0)
			{
				m_sameName = false;
				//Create thread
				std::thread(CreateProcessHandler()).detach();
				m_frameThread = std::thread(&TCPClient::ProcessFrameThread,this);

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
	delete[]buffer;

	if (stringMessage.compare(InformationStrings::StopVideo().toStdString()) == 0)
	{
		Q_EMIT stopShowVideo();

		ResetDecoder();
		m_clearQueue = true;
	}
	else if (stringMessage.compare(InformationStrings::StartVideo().toStdString()) == 0)
	{
		Q_EMIT startShowVideo();
	}
	else if (stringMessage.compare(InformationStrings::List().toStdString()) == 0)
	{
		PacketType packet;

		int resultPacket = recv(m_connection, (char*)&packet, sizeof(packet), NULL);
		if (resultPacket == SOCKET_ERROR)
		{
			return;
		}

		if (packet == PacketType::P_InformationMessage)
		{
			std::string listOfClients;
			ReceiveMessage(listOfClients);

			Q_EMIT updateList(QString::fromStdString(listOfClients));
		}
	}

	message = stringMessage;


	return;
}
void TCPClient::ReceiveMessage(std::string & message)
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

	message = buffer;

	delete[]buffer;

	return;
}
void TCPClient::CreateFaceDetector(QString path)
{
	m_faceDetector = std::make_unique<FaceLandmarkDetector>(path.toStdString());
}
void TCPClient::ResetDecoder()
{
	m_decoder.release();
	m_decoder = std::make_unique<H264Decoder>();
}
void TCPClient::ProcessFrameThread()
{
	while (!m_terminating)
	{
		while (!m_queueFrames.empty() && !m_clearQueue)
		{
			m_currentFrame = m_queueFrames.front();
			m_faceDetector->Process(m_currentFrame);
			recieveEventFrame();
			m_queueFrames.pop_front();
		}
		
		if (m_clearQueue)
		{
			m_queueFrames.clear();
			m_clearQueue = false;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	return;
}

void TCPClient::SendMessageWithoutName(std::string message)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	PacketType packet = PacketType::P_ChatMessage;

	int resultPacket = send(m_connection, (char*)&packet, sizeof(packet), NULL);
	if (resultPacket == SOCKET_ERROR)
	{
		qDebug() << "SendMessageWithoutName: error send packet";
		return;
	}
	int messageSize = (int)(message.size());

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

	PacketType packet = PacketType::P_ChatMessage;

	int resultPacket = send(m_connection, (char*)&packet, sizeof(packet), NULL);
	if(resultPacket == SOCKET_ERROR)
	{
		qDebug() << "SendMessage: error send packet";
		return;
	}

	std::string tempName = m_name + ": ";
	std::string mesageWithName = message.insert(0, tempName);
	int messageSize = (int)(mesageWithName.size());

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

void TCPClient::SendFrame(std::vector<uchar> buffer)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	PacketType packet = PacketType::P_FrameMessage;
	//packet
	int resultPacket = send(m_connection, (char*)&packet, sizeof(packet), NULL);

	if (resultPacket == SOCKET_ERROR)
	{
		qDebug() << "SendFrame: error send packet";
		return;
	}
	int bufferSize = (int)(buffer.size());

	//send int
	int resultInt = send(m_connection, (char*)&bufferSize, sizeof(int), NULL);

	if (resultInt == SOCKET_ERROR)
	{
		qDebug() << "SendFrame: error send size";
		return;
	}
	//send frame
	int resultFrame = send(m_connection,(char*)buffer.data(), bufferSize, NULL);

	if (resultInt == SOCKET_ERROR)
	{
		qDebug() << "SendFrame: error send frame";
		return;
	}
	
	return;
}

void TCPClient::SendFrame(std::vector<uint8_t> data, int size)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	PacketType packet = PacketType::P_FrameMessage;
	//packet
	int resultPacket = send(m_connection, (char*)&packet, sizeof(packet), NULL);

	if (resultPacket == SOCKET_ERROR)
	{
		qDebug() << "SendFrame: error send packet";
		return;
	}
	int bufferSize = size;

	//send int
	int resultInt = send(m_connection, (char*)&bufferSize, sizeof(int), NULL);

	if (resultInt == SOCKET_ERROR)
	{
		qDebug() << "SendFrame: error send size";
		return;
	}
	//send frame
	int resultFrame = send(m_connection,reinterpret_cast<char*>(data.data()), bufferSize, NULL);

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

	PacketType packet = PacketType::P_AudioMessage;

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

	return;
}

void TCPClient::SendInformationMessage(std::string message)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	PacketType packet = PacketType::P_InformationMessage;

	int resultPacket = send(m_connection, (char*)&packet, sizeof(packet), NULL);
	if (resultPacket == SOCKET_ERROR)
	{
		qDebug() << "SendInformationMessage: error send packet";
		return;
	}

	int messageSize = (int)(message.size());

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

void TCPClient::SetAppPath(QString path)
{
	m_path = path;
	CreateFaceDetector(m_path);
}

void TCPClient::StopThread()
{
	m_terminating = true;
}

std::tuple<std::string, std::string, int> TCPClient::GetClientInformation() const
{
	return std::make_tuple(m_name, m_ip,m_port);
}

cv::Mat TCPClient::GetCurrentFrame() const
{
	return m_currentFrame;
}

bool TCPClient::IsSameName()
{
	return m_sameName;
}

void TCPClient::RecieveFrame()
{
	std::vector<uchar> buffer;

	int frameSize;
	int resultInt = recv(m_connection, (char*)&frameSize, sizeof(int), NULL);

	if (resultInt == SOCKET_ERROR)
	{
		qDebug() << "RecieveFrame: error  size";
		return;
	}

	buffer.resize(frameSize);

	int resultFrame = recv(m_connection, (char*)buffer.data(), frameSize, MSG_WAITALL);

	if (resultFrame == SOCKET_ERROR)
	{
		qDebug() << "RecieveFrame: error  frame";
		return;
	}

	std::shared_ptr<uint8_t*> data = std::make_shared<uint8_t*>();
	*data = buffer.data();
	

	if (m_decoder->Decode(*data, frameSize))
	{
		m_queueFrames.push_back(m_decoder->GetFrame());
	}


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

	delete []buffer;

	return;
}
