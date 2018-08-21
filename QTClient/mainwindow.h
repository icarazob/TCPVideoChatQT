#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "TCPClient.h"
#include "loginwindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QString port, QString ip, QString name);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
	TCPClient *m_client;
	QString m_ip;
	QString m_port;
	QString m_name;
public slots:
	void exit();
	void UpdatePlain();
	void UpdatePlainText(QString message);

};

#endif // MAINWINDOW_H
