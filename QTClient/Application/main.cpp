#include <QApplication>
#include "Controllers/MainController.h"

#include "Ui/loginwindow.h"




int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar);



	MainController mainController;
	LoginWindow loginWindow;

	mainController.SetView(&loginWindow);
	mainController.Start();

	return app.exec();
}