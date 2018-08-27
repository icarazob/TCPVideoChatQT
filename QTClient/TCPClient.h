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
		P_FrameMessage,
		P_AudioMessage
	};
private:
	bool ProcessPacket(PacketType &packet);
	void InitializeWSA();
	std::function<void (void)> CreateProcessHandler();
	void CreateSocket();
	void RecieveFrame();
	void RecieveAudio();
public:
	TCPClient(int port, const char *ip,std::string name);
	~TCPClient();
	bool Connect();

	void RecieveMessage();
	void SendMessage(std::string message);
	void SendFrame(cv::Mat frame);
	void SendAudio(QByteArray buffer,int lengt);
	
	cv::Mat GetCurrentFrame();
	
signals:
	void recieveEvent(QString message);
	void recieveEventFrame();
	void recieveEventAudio(QByteArray,int);
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