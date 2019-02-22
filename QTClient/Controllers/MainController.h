#pragma once
#include <QObject>
#include <memory>

class LoginWindow;
class MainWindow;
class TCPClient;
class MainWindowController;


class MainController : public QObject
{
	Q_OBJECT
public:
	explicit MainController(QObject *parent = nullptr);

	void SetView(LoginWindow *loginWindow);
	void Start();

	~MainController();

	public slots:
		void ShowMainWindow();

private:

	void Initialize(LoginWindow *loginWindow);

	LoginWindow *m_loginWindow;
	MainWindow  *m_mainWindow;

	std::unique_ptr<TCPClient> m_tcpClient;
	std::unique_ptr<MainWindowController> m_mainWindowController;

	QString m_appPath;
};

