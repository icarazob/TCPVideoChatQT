#include "mainwindow.h"
#include "loginwindow.h"
#include <QApplication>



int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	loginwindow loginWindow;
	loginWindow.exec();
	
	if (!loginWindow.GetStatus())
	{
		return -1;
	}

	MainWindow mainwindow(
		loginWindow.GetClientPort(),
		loginWindow.GetClientIp(),
		loginWindow.GetClientName(),
		loginWindow.GetTCPClient()
	);

	mainwindow.show();



	return app.exec();
}