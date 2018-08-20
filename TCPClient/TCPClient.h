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

private:
	WSADATA wsaData;
	SOCKADDR_IN addr;
	SOCKET connection;
	int port;
	std::string ip;
};