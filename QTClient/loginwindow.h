#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QDialog>
#include <QString>
#include <memory>
#include "TCPClient.h"

namespace Ui {
class loginwindow;
}

class loginwindow : public QDialog
{
    Q_OBJECT
private:
	bool CheckIp();
	bool CheckPort();
	bool CheckName();
	void ReadXmlSettings(QString path);
public:
    explicit loginwindow(QDialog *parent = 0);
	
	QString GetClientIp();
	QString GetClientPort();
	QString GetClientName();
	bool GetStatus();

	std::shared_ptr<TCPClient> GetTCPClient();

	bool CheckOnValidInputData();

    ~loginwindow();

private:
    Ui::loginwindow *ui;
	QString m_ip;
	QString m_port;
	QString m_name;
	std::shared_ptr<TCPClient> m_client;
	bool m_status;
public slots:
	void exit();
};

#endif // LOGINWINDOW_H
