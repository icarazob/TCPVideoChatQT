#pragma  once
#pragma comment(lib,"ws2_32.lib")
#include <iostream>
#include <WinSock2.h>
#include <string>
#include <functional>
#include <QObject>
#include <QString>
#include <mutex>
#include <tuple>
#include <opencv2/opencv.hpp>
#include <condition_variable>
#include "SharedQueue.h"




class Detector;
class H264Decoder;

class TCPClient: public QObject{
	Q_OBJECT
public:
	enum class PacketType {
		P_ChatMessage,
		P_FrameMessage,
		P_AudioMessage,
		P_InformationMessage
	};

private:
	void InitializeWSA();
	void CreateSocket();
	std::function<void (void)> CreateProcessHandler();

	bool ProcessPacket(PacketType &packet);
	void RecieveFrame();
	void RecieveAudio();
	void RecieveMessage();
	void RecieveInformationMessage(std::string &message);
	void ReceiveMessage(std::string &message);

	void ResetDecoder();

	template<typename type>
	void ResetDetector();

public:
	explicit TCPClient(int port, const char *ip,std::string name);
	~TCPClient();

	bool Connect();

	void SendMessageWithoutName(std::string message);
	void SendMessage(std::string message);
	void SendFrame(std::vector<uchar> buffer);
	void SendFrame(std::vector<uint8_t> data, int size);
	void SendAudio(QByteArray buffer,int lengt);
	void SendInformationMessage(std::string message);

	std::tuple<std::string, std::string, int> GetClientInformation() const;
	cv::Mat GetCurrentFrame() const;

	void SetAppPath(QString path);
	void StopThread();
	bool IsSameName();

	void ChangeDetector(int type);
	void CloseDetector();
	
Q_SIGNALS:
	void recieveEventMessage(QString message);
	void recieveEventFrame();
	void recieveEventAudio(QByteArray, int);

	void startShowVideo();
	void stopShowVideo();

	void updateList(QString);

private:

	void ProcessFrameThread();

	QString m_path;
	WSADATA m_wsaData;
	SOCKADDR_IN m_addr;
	SOCKET m_connection;

	int m_port;
	cv::Mat m_currentFrame;
	std::string m_name;
	std::string m_ip;

	std::thread m_frameThread;
	std::mutex m_frameThreadMutex;
	std::mutex m_mutex;

	bool m_sameName = false;
	bool m_terminating = false;
	bool m_clearQueue = false;
	bool m_faceDetectorRun = true;

	std::unique_ptr<Detector> m_faceDetector;
	std::unique_ptr<H264Decoder> m_decoder;
	std::unique_ptr<SharedQueue<cv::Mat>> m_queueFrames;
};


