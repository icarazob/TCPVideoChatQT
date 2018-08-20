#pragma  once
#include <iostream>
#pragma comment(lib,"ws2_32.lib")
#include <WinSock2.h>
#include <string>



class TCPClient {
private:
	void InitializeWSA();

	void CreateSocket();
public:
	TCPClient(int port, const char *ip);

	bool Connect();

	void RecieveMessage();
	void SendMessage(std::string message);

private:
	WSADATA m_wsaData;
	SOCKADDR_IN m_addr;
	SOCKET m_connection;
	int m_port;
	std::string m_ip;
};