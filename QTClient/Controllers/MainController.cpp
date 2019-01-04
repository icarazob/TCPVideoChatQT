#include "MainController.h"
#include "MainWindowController.h"
#include "Ui/loginwindow.h"
#include "Ui/mainwindow.h"
#include "Core/TCPClient.h"
#include "QApplication"


void MainController::SetView(LoginWindow * loginWindow)
{
	Q_ASSERT(loginWindow);
	Q_ASSERT(loginWindow->GetMainWindow());

	m_loginWindow = loginWindow;
	m_mainWindow = loginWindow->GetMainWindow();

	QObject::connect(m_loginWindow, &LoginWindow::ClientConnected, this, &MainController::ShowMainWindow);
}

void MainController::Start()
{
	m_loginWindow->show();
	m_loginWindow->adjustSize();
}

MainController::MainController(QObject *parent) :
	QObject(parent),
	m_loginWindow(nullptr),
	m_mainWindow(nullptr),
	m_tcpClient(nullptr),
	m_mainWindowController(nullptr)
{
	QString appPath = QCoreApplication::applicationDirPath();
	m_mainWindowController = std::make_unique<MainWindowController>(appPath);
}

MainController::~MainController()
{

}

void MainController::ShowMainWindow()
{
	auto tcpClient = m_loginWindow->GetTCPClient();

	m_mainWindowController->SetTCPClient(std::move(tcpClient));
	m_mainWindowController->SetView(m_mainWindow);

	m_mainWindow->adjustSize();
	m_mainWindow->show();
}
