#include <QApplication>
#include "Controllers/MainController.h"
#include "Ui/loginwindow.h"
#include <opencv2/opencv.hpp>


int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	
	MainController mainController;
	LoginWindow loginWindow;

	mainController.SetView(&loginWindow);
	mainController.Start();

	return app.exec();
}