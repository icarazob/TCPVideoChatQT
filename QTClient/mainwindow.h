#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "TCPClient.h"
#include "loginwindow.h"
#include <functional>
#include <memory>
#include <thread>
#include <iostream>
#include <unordered_map>
#include <opencv2\opencv.hpp>

namespace Ui {
class MainWindow;
}

class MainWindow : public QObject, public QMainWindow
{
    Q_OBJECT
private:
	std::function<void (void)> GetVideoHandler();

	typedef std::unordered_map<std::string, std::thread> ThreadMap;

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
public slots:
	void exit();
	void UpdatePlain();
	void UpdatePlainText(QString message);
	void ShowFrame();
private slots:
	void StartVideoStream();
	void StopVideoStream();
};

#endif // MAINWINDOW_H
