#pragma  once
#include <iostream>
#pragma comment(lib,"ws2_32.lib")
#include <WinSock2.h>
#include <string>
#include <functional>
#include <QObject>
#include <QString>
#include <opencv2/opencv.hpp>
#include <mutex>



class TCPClient: public QObject{
	Q_OBJECT
public:
	enum PacketType {
		P_ChatMessage,
		P_FrameMessage
	};
private:
	bool ProcessPacket(PacketType &packet);
	void InitializeWSA();
	std::function<void (void)> CreateProcessHandler();
	void CreateSocket();
	void RecieveFrame();
public:
	TCPClient(int port, const char *ip,std::string name);

	bool Connect();

	void RecieveMessage();
	void SendMessage(std::string message);
	void SendFrame(cv::Mat frame);
	
	cv::Mat GetCurrentFrame();
	
signals:
	void recieveEvent(QString message);
	void recieveEventFrame();
private:
	WSADATA m_wsaData;
	SOCKADDR_IN m_addr;
	SOCKET m_connection;
	int m_port;
	std::string m_name;
	std::string m_ip;
	std::mutex m_mutex;
	cv::Mat m_currentFrame;
};