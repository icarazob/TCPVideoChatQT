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
		P_AudioMessage,
		P_InformationMessage
	};
private:
	bool ProcessPacket(PacketType &packet);
	void InitializeWSA();
	std::function<void (void)> CreateProcessHandler();
	void CreateSocket();
	void RecieveFrame();
	void RecieveAudio();
	void RecieveMessage();
	void RecieveInformationMessage(std::string &message);
public:
	TCPClient(int port, const char *ip,std::string name);
	~TCPClient();
	bool Connect();

	void SendMessageWithoutName(std::string message);
	void SendMessage(std::string message);
	void SendFrame(cv::Mat frame);
	void SendAudio(QByteArray buffer,int lengt);
	void SendInformationMessage(std::string message);

	
	cv::Mat GetCurrentFrame();
	
signals:
	void recieveEventMessage(QString message);
	void recieveEventFrame();
	void recieveEventAudio(QByteArray,int);
	void clearLabel();
private:
	WSADATA m_wsaData;
	SOCKADDR_IN m_addr;
	SOCKET m_connection;
	int m_port;
	std::string m_name;
	std::string m_ip;
	std::mutex m_mutex;
	cv::Mat m_currentFrame;
	std::condition_variable m_cv;
	bool m_proceed = true;
	
};