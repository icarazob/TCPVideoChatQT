#pragma  once
#include <iostream>
#pragma comment(lib,"ws2_32.lib")
#include <WinSock2.h>
#include <string>
#include <functional>
#include <QObject>
#include <QString>



class TCPClient: public QObject{
	Q_OBJECT
private:
	void InitializeWSA();
	std::function<void (void)> CreateMessageHandler();
	void CreateSocket();
public:
	TCPClient(int port, const char *ip,std::string name);

	bool Connect();

	void RecieveMessage();
	void SendMessage(std::string message) const;
signals:
	void recieveEvent(QString message);
private:
	WSADATA m_wsaData;
	SOCKADDR_IN m_addr;
	SOCKET m_connection;
	int m_port;
	std::string m_name;
	std::string m_ip;
};