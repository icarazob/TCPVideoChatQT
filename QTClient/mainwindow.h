#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "TCPClient.h"
#include "loginwindow.h"
#include "NativeFrameLabel.h"
#include "AudioProcessor.h"
#include "PopUpNotification.h"
#include <functional>
#include <memory>
#include <thread>
#include <iostream>
#include <unordered_map>
#include <opencv2\opencv.hpp>

namespace Ui {
class MainWindow;
}

class MainWindow :public QObject,public QMainWindow
{
    Q_OBJECT
private:
	std::function<void (void)> GetVideoHandler();

	typedef std::unordered_map<std::string, std::thread> ThreadMap;
	bool eventFilter(QObject *watched, QEvent *event);

	void SetStatusVideoRead(bool value);
	bool GetStatusVideoRead();
	void ChangeMicrophoneIcon(bool status);
	void GetHistoryWithClient(std::string clientName);
	void ClearPlainText();

	void CompressFrame(cv::Mat& frame, std::vector<uchar>& buffer);
	void SetupIcons();
	void SetupElements();
	void PlaySound(QString path);
public:
    explicit MainWindow(QString port, QString ip, QString name,std::unique_ptr<TCPClient> client);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
	std::unique_ptr<TCPClient> m_client;
	std::unique_ptr<PopUpNotification> m_notification;
	cv::VideoCapture m_capture;
	QString m_ip;
	QString m_port;
	QString m_name;
	ThreadMap threadMap;
	std::shared_ptr<NativeFrameLabel> m_nativeFrameLabel;

	std::mutex m_videoMutex;
	std::unique_ptr<AudioProcessor> m_audio;
	QString m_path;
	
	bool m_shouldRead = false;
	bool m_lastStateAudioButton = false;
public slots:
	void exit();
	void UpdatePlain();
	void UpdatePlainText(QString message);
	void ShowFrame();
	void ClientStartVideo();
signals:
	void videoStream(bool);
private slots:
	void StartVideoStream();
	void StopVideoStream();
	void TurnAudio();
	void SendAudio(QByteArray buffer, int length);
	void ProcessAudioData(QByteArray data, int length);
	void ClearFrameLabel();
	void UpdateList(QString listOfClients);
	void ListItemClicked();
};

#endif // MAINWINDOW_H
