#pragma once
#include <iostream>
#include <WinSock2.h>
#include <vector>
#include <thread>
#pragma comment(lib,"ws2_32.lib")

#define _WINSOCK_DEPRECATED_NO_WARNINGS


class TCPServer {
private:
	void InitializeWSA();
	void CreateBindListenSocket();
	void RecieveMessage();
	void SendMessage(SOCKET client,const std::string message);
public:
	TCPServer(int port, const char *ip);

	void Run();
	void ShowServerInformation();
	void WorkClient();

private:
	WSADATA m_wsaData;
	SOCKET m_listenSocket;
	SOCKADDR_IN m_addr;
	std::string m_ip;
	std::vector<SOCKET> m_clients;
	std::vector<std::thread> m_threads;
	int m_port;
};