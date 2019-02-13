#include "loginwindow.h"
#include "mainwindow.h"
#include "ui_loginwindow.h"
#include "Core/TCPClient.h"
#include <QFile>
#include <QXmlStreamReader>
#include <QCoreApplication>
#include <QRegExpValidator>
#include <QTimer>

bool LoginWindow::CheckIp()
{
	if (m_ip.isEmpty())
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool LoginWindow::CheckPort()
{
	bool ok;
	int port = m_port.split(" ")[0].toInt(&ok);
	
	if (!ok)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool LoginWindow::CheckName()
{
	if (m_name.isEmpty())
	{
		return false;
	}
	else
	{
		return true;
	}
}

void LoginWindow::ReadXmlSettings(QString path)
{
	QFile file(path);

	if (!file.open(QFile::ReadOnly | QFile::Text))
	{
		return;
	}

	QXmlStreamReader reader(&file);
	
	if (reader.readNextStartElement())
	{
		if (reader.name() == "settings")
		{
			while (reader.readNextStartElement())
			{
				if (reader.name() == "port")
				{
					QString port = reader.readElementText();
					ui->portLineEdit->setText(port);
				}
				else if (reader.name() == "ip")
				{
					QString ip = reader.readElementText();
					ui->ipLineEdit->setText(ip);
				}
				else
				{
					reader.skipCurrentElement();
				}
			}
		}
	}
}

LoginWindow::LoginWindow(QMainWindow *parent) :
	QMainWindow(parent),
    ui(new Ui::loginwindow),
	m_mainWindow(new MainWindow)
{
    ui->setupUi(this);

	QString xmlPath = QCoreApplication::applicationDirPath();
	xmlPath = xmlPath + "/settings/XMLsettings.xml";

	ReadXmlSettings(xmlPath);

	QRegExp exp("([à-ÿ]|[a-z]|[1-9]|[A-Z]|[À-ß]){1,15}");
	QRegExpValidator *validator = new QRegExpValidator(exp, this);
	ui->nameLineEdit->setValidator(validator);


	QTimer::singleShot(0, ui->nameLineEdit, SLOT(setFocus()));
	QObject::connect(ui->okButton, SIGNAL(clicked()), SLOT(exit()));
}

QString LoginWindow::GetClientIp()
{
	return m_ip;
}

QString LoginWindow::GetClientPort()
{
	return m_port;
}
QString LoginWindow::GetClientName()
{
	return m_name;
}

bool LoginWindow::GetStatus()
{
	return m_status;
}

MainWindow* LoginWindow::GetMainWindow() const
{
	return m_mainWindow;
}

std::unique_ptr<TCPClient> LoginWindow::GetTCPClient()
{
	return std::move(m_client);
}

bool LoginWindow::CheckOnValidInputData()
{
	if (!CheckName())
	{
		return false;
	}

	if (!CheckPort())
	{
		return false;
	}
	if (!CheckIp())
	{
		return false;
	}

	return true;
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::exit()
{
	m_ip = ui->ipLineEdit->text();
	m_port = ui->portLineEdit->text();
	m_name = ui->nameLineEdit->text();

	if (CheckOnValidInputData())
	{
		m_client.reset(nullptr);

		m_client = std::make_unique<TCPClient>(m_port.split(" ")[0].toInt(), m_ip.toUtf8().constData(), m_name.toUtf8().constData());

		if (m_client->Connect())
		{
			m_status = true;
			Q_EMIT ClientConnected();
			this->close();
		}
		else
		{
			m_status = false;

			if (m_client->IsSameName())
			{
				ui->connectLabel->setText("Client with the same name exist!");
				ui->connectLabel->setStyleSheet("color:red");
			}
			else
			{
				ui->connectLabel->setText("Can't connected!");
				ui->connectLabel->setStyleSheet("color:red");
			}
		}
		
	}
	else
	{
		ui->okButton->setStyleSheet("background-color:red");
	}
}
