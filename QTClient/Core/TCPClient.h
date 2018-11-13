#pragma  once
#include <iostream>
#pragma comment(lib,"ws2_32.lib")
#include <WinSock2.h>
#include <string>
#include <functional>
#include <QObject>
#include <QString>
#include <mutex>
#include <tuple>



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
	void ReceiveMessage(std::string &message);
public:
	explicit TCPClient(int port, const char *ip,std::string name);
	~TCPClient();
	bool Connect();

	void SendMessageWithoutName(std::string message);
	void SendMessage(std::string message);
	void SendFrame(std::vector<uchar> buffer);
	void SendAudio(QByteArray buffer,int lengt);
	void SendInformationMessage(std::string message);

	std::tuple<std::string, std::string, int> GetClientInformation() const;

	cv::Mat GetCurrentFrame();
	bool IsSameName();
	
Q_SIGNALS:
	void recieveEventMessage(QString message);
	void recieveEventFrame();
	void recieveEventAudio(QByteArray,int);
	void clearLabel();
	void updateList(QString);
	void startVideo();
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
	bool m_sameName = false;
};