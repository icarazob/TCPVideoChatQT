#pragma once
#include <iostream>
#include <WinSock2.h>
#include <vector>
#include <thread>
#include <mutex>
#include "DbLib/db.h"
#pragma comment(lib,"ws2_32.lib")

#define _WINSOCK_DEPRECATED_NO_WARNINGS

namespace cv {
	class Mat;
}

namespace Server {

	class TCPServer {
	private:

		enum PacketType {
			P_ChatMessage,
			P_FrameMessage,
			P_AudioMessage,
			P_InformationMessage,
			P_FrameMultipleMessage
		};

		bool ProcessPacket(SOCKET client, PacketType &packet, std::string userName);
		void InitializeWSA();
		void SetupSockaddr();
		void CreateBindListenSocket();
		void SendMessage(SOCKET client, const std::string message);
		void SendAllMessage(const std::string message);
		bool RecieveMessage(SOCKET client, std::string &message);
		void DeleteClient(SOCKET client);
		std::function<void(int)> CreateHandler();
		void ShowServerInformation();
		void RecieveFrame(SOCKET client, std::vector<uchar>& data);
		bool SendFrame(SOCKET client, std::vector<uchar> data);
		bool ReceiveAudio(SOCKET client, char **buffer, int &length);
		void SendAudio(SOCKET client, char *buffer, int length);
		bool ProcessInformationMessage(SOCKET client);
		void SendInformationMessage(SOCKET client, std::string message);
		void SendAllWithoutClientInformationMessage(SOCKET client, std::string message);
		void SendAllInformationMessage(std::string message);
		void RecievePacket(SOCKET client, PacketType &packet);
		void SendClientsList();
		bool ReadSettings();
		void SaveMessageHistoryOfClient(SOCKET client);
		bool SendFrameMultipleMode(SOCKET client, std::vector<uchar> data, std::string userName);
	public:
		TCPServer();
		explicit TCPServer(int port, const char *ip);

		bool Run();

		~TCPServer();

	private:
		bool m_multipleMode = false;

		WSADATA m_wsaData;
		SOCKET m_listenSocket = 0;
		std::vector<SOCKET> m_clients;
		std::vector<std::string> m_names;
		SOCKADDR_IN m_addr;

		std::mutex m_mutex;

		QString m_applicationPath;
		
		int m_videoCounter = 0;

		std::string m_ip;
		int m_port = 5000;
	};
}
