#pragma once
#include <iostream>
#include <WinSock2.h>
#include <vector>
#include <thread>
#include <mutex>
#pragma comment(lib,"ws2_32.lib")

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define MAX_COUNT 100


class TCPServer {
private:
	void InitializeWSA();
	void CreateBindListenSocket();
	void SendMessage(SOCKET client, const std::string message);
public:
	TCPServer(int port, const char *ip);

	void Run();
	void ShowServerInformation();
	void Igor();

	~TCPServer();

private:
	WSADATA m_wsaData;
	SOCKET m_listenSocket;
	std::vector<SOCKET> m_clients;
	SOCKADDR_IN m_addr;
	std::string m_ip;
	std::mutex m_mutex;
	int m_port;
};