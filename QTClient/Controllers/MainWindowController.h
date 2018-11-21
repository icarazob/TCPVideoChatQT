#pragma once
#include <QObject>
#include <memory>
#include <thread>
#include <unordered_map>
#include <future>
#include <functional>
#include <tuple>
#include <opencv2/opencv.hpp>

class MainWindow;
class TCPClient;
class AudioProcessor;


class  MainWindowController: public QObject
{
	Q_OBJECT
private:

	struct ClientInformation
	{
		std::string name;
		std::string ip;
		int port;
	};

	std::function<void(void)> GetVideoHandler();

	void CompressFrame(const cv::Mat& frame, std::vector<uchar>& buffer);

	typedef std::unordered_map<std::string, std::thread> ThreadMap;

	void SetClientInformation(std::tuple<std::string, std::string, int> information);

public:
	MainWindowController(QString path);
	void SetView(MainWindow *view);
	void SetTCPClient(std::unique_ptr<TCPClient> client);
	~MainWindowController();

private slots:
	void ViewStopShowVideo();
	void ViewUpdatePlainText(QString message);
	void ViewUpdateList(QString listOfClients);
	void ViewShowFrame();
	void ViewStartShowVideo();

	void ProcessAudioData(QByteArray data, int length);
	void TurnAudioSlot(bool state);
	void TurnVideoSlot(bool state);


	void SendMessageSlot(QString message);
	void SendAudioSlot(QByteArray buffer, int length);
	void SendFrameSlot(std::vector<uchar> data);
	void SendInformationSlot(QString message);
private:
	MainWindow *m_view;
	QString m_appPath;

	std::unique_ptr<TCPClient> m_tcpClient;
	std::unique_ptr<AudioProcessor> m_audioProcesscor;
	std::unique_ptr<cv::VideoCapture> m_videoCapture;

	ThreadMap m_threadMap;
	bool m_shouldReadFrame = false;
	ClientInformation m_clientInformation;

};


