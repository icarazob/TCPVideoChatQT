#pragma once
#include <iostream>
#include <WinSock2.h>
#include <vector>
#include <thread>
#include <mutex>
#pragma comment(lib,"ws2_32.lib")

#define _WINSOCK_DEPRECATED_NO_WARNINGS

namespace cv {
	class Mat;
}
class TCPServer {
private:
	enum PacketType {
		P_ChatMessage,
		P_FrameMessage,
		P_AudioMessage
	};


	bool ProcessPacket(SOCKET client,PacketType &packet);
	void InitializeWSA();
	void CreateBindListenSocket();
	void SendMessage(SOCKET client, const std::string message);
	bool RecieveMessage(SOCKET client, std::string &message);
	void DeleteClient(SOCKET client);
	std::function<void (int)> CreateHandler();
	void ShowServerInformation();
	cv::Mat RecieveFrame(SOCKET client);
	void SendFrame(SOCKET client, cv::Mat frame);
	bool ReceiveAudio(SOCKET client,char **buffer,int &length);
	void SendAudio(SOCKET client, char *buffer,int length);

public:
	TCPServer(int port, const char *ip);

	bool Run();

	~TCPServer();

private:
	WSADATA m_wsaData;
	SOCKET m_listenSocket = 0;
	std::vector<SOCKET> m_clients;
	SOCKADDR_IN m_addr;
	std::string m_ip;
	std::mutex m_mutex;
	int m_port= 5000;
};