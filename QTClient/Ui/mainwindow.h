#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include <iostream>
#include <QPOinter>
#include <QScrollBar>

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
	void UpdateNativeLabel();
	bool eventFilter(QObject *watched, QEvent *event);

	void AddItemToList(QString text, bool isClientMessage);
	void ChangeMicrophoneIcon(bool status);
	void GetHistoryWithClient(std::string clientName);
	void ClearPlainText();
	

	void SetupIcons();
	void SetupElements();
	void PlayNotificationSound(QString path);
	bool FileExist(QString path);

	void resizeEvent(QResizeEvent *event);

	QScrollBar* GetScrollBar();

	void SetImageLabel(const QChar character);

public:
    explicit MainWindow(QMainWindow *parent = nullptr);

	void StopShowVideo();
	void ClearNativeFrameLabel();

	void UpdatePlainText(QString message);
	void UpdateList(QString listOfClients);
	void StartShowVideo();
	void ShowFrame(const cv::Mat &copyFrame);
	void ShowFrameOnNativeLabel(const cv::Mat& frame);
	QSize GetFrameLabelSize() const;
	QSize GetNativeLabelSize() const;
	void SetNameLabel(QString name);

	void SetAppPath(QString path);

    ~MainWindow();

Q_SIGNALS:
	void SendMessageSignal(QString message);
	void SendFrameSignal(QByteArray data);
	void TurnAudioSignal(bool state);
	void videoStream(bool);
	void SendInformationSignal(QString message);
	void TurnVideoSignal(bool state);
	void AboutClickedSignal();
	void SettingsClickedSignal();
	void WindowClosedSignal();

public slots:
	void UpdatePlain();
	void StartVideoStream();
	void StopVideoStream();
	void TurnAudio();
	void ListItemClicked();

private:
    Ui::MainWindow *ui;
	QString m_path;
	QString m_soundPath;
	
	std::unique_ptr<PopUpNotification> m_notification;
	std::shared_ptr<NativeFrameLabel> m_nativeFrameLabel;

	bool m_lastStateAudioButton = false;
	bool m_stopShowVideo = false;

	QPointer<QAction> m_Act = nullptr;
};

#endif // MAINWINDOW_H
