#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "TCPClient.h"
#include "loginwindow.h"
#include <functional>
#include <memory>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
	std::function<void (void)> GetVideoHandler();
public:
    explicit MainWindow(QString port, QString ip, QString name);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
	std::shared_ptr<TCPClient> m_client;
	QString m_ip;
	QString m_port;
	QString m_name;
public slots:
	void exit();
	void UpdatePlain();
	void UpdatePlainText(QString message);

};

#endif // MAINWINDOW_H
