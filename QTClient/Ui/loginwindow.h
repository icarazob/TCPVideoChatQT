#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H
#include <QMainWindow>
#include <QDialog>
#include <QString>
#include <memory>



namespace Ui {
class loginwindow;
}

class TCPClient;
class MainWindow;

class LoginWindow : public QMainWindow
{
    Q_OBJECT
private:
	bool CheckIp();
	bool CheckPort();
	bool CheckName();
	void ReadXmlSettings(QString path);
	bool CheckOnValidInputData();
public:
    explicit LoginWindow(QMainWindow *parent = 0);
	~LoginWindow();

	QString GetClientIp();
	QString GetClientPort();
	QString GetClientName();
	bool GetStatus();
	MainWindow* GetMainWindow() const;
	std::unique_ptr<TCPClient> GetTCPClient();

	void SetAppPath(QString path);
	void SetIcon();

Q_SIGNALS:
	void ClientConnected();
private:
    Ui::loginwindow *ui;
	MainWindow *m_mainWindow;
	QString m_ip;
	QString m_port;
	QString m_name;
	QString m_appPath;
	bool m_status = false;

	std::unique_ptr<TCPClient> m_client;

public slots:
	void exit();

};

#endif // LOGINWINDOW_H
