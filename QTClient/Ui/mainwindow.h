#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include <iostream>

namespace Ui {
class MainWindow;
}

namespace cv {
	class Mat;
}

class NativeFrameLabel;
class PopUpNotification;



class MainWindow :public QMainWindow
{
    Q_OBJECT
private:



	bool eventFilter(QObject *watched, QEvent *event);

	void ChangeMicrophoneIcon(bool status);
	void GetHistoryWithClient(std::string clientName);
	void ClearPlainText();


	void SetupIcons();
	void SetupElements();
	void PlayNotificationSound(QString path);

public:
    explicit MainWindow(QMainWindow *parent = nullptr);

	void ClearFrameLabel();
	void UpdatePlainText(QString message);
	void UpdateList(QString listOfClients);
	void ClientStartVideo();
	void ShowFrame(cv::Mat copyFrame);
	void ShowFrameOnNativeLabel(cv::Mat frame);
	QSize GetFrameLabelSize() const;
	QSize GetNativeLabelSize() const;

	void SetNameLabel(std::string name);
	void SetAppPath(QString path);
    ~MainWindow();

Q_SIGNALS:
	void exit();
	void SendMessageSignal(QString message);
	void SendFrameSignal(QByteArray data);
	void TurnAudioSignal(bool state);
	void videoStream(bool);
	void SendInformationSignal(QString message);
	void TurnVideoSignal(bool state);

public slots:
	void UpdatePlain();
	void StartVideoStream();
	void StopVideoStream();
	void TurnAudio();
	void ListItemClicked();



private:
    Ui::MainWindow *ui;
	std::unique_ptr<PopUpNotification> m_notification;
	std::shared_ptr<NativeFrameLabel> m_nativeFrameLabel;
	QString m_path;

	bool m_lastStateAudioButton = false;
};

#endif // MAINWINDOW_H
