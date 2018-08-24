#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "TCPClient.h"
#include "loginwindow.h"
#include "NativeFrameLabel.h"
#include "AudioProcessor.h"
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

public:
    explicit MainWindow(QString port, QString ip, QString name,std::shared_ptr<TCPClient> client);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
	std::shared_ptr<TCPClient> m_client;
	cv::VideoCapture m_capture;
	QString m_ip;
	QString m_port;
	QString m_name;
	ThreadMap threadMap;
	NativeFrameLabel *m_nativeFrameLabel;

	std::mutex m_videoMutex;
	std::shared_ptr<AudioProcessor> m_audio;
	
	bool m_shouldRead = false;
	bool m_lastStateAudioButton = false;
public slots:
	void exit();
	void UpdatePlain();
	void UpdatePlainText(QString message);
	void ShowFrame();
signals:
	void videoStream(bool);
private slots:
	void StartVideoStream();
	void StopVideoStream();
	void TurnAudio();
};

#endif // MAINWINDOW_H
