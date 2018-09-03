#include "loginwindow.h"
#include "ui_loginwindow.h"
#include <QFile>
#include <QXmlStreamReader>
#include <QCoreApplication>
#include <QRegExpValidator>
#include <QTimer>

bool loginwindow::CheckIp()
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

bool loginwindow::CheckPort()
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

bool loginwindow::CheckName()
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

void loginwindow::ReadXmlSettings(QString path)
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

loginwindow::loginwindow(QDialog *parent) :
	QDialog(parent),
    ui(new Ui::loginwindow)
{
    ui->setupUi(this);

	QString xmlPath = QCoreApplication::applicationDirPath();
	xmlPath = xmlPath + "/settings/XMLsettings.xml";

	ReadXmlSettings(xmlPath);


	QRegExp exp("([a-z]|[1-9]|[A-Z]){1,15}");
	QRegExpValidator *validator = new QRegExpValidator(exp, this);
	ui->nameLineEdit->setValidator(validator);



	QTimer::singleShot(0, ui->nameLineEdit, SLOT(setFocus()));
	QObject::connect(ui->okButton, SIGNAL(clicked()), SLOT(exit()));


}

QString loginwindow::GetClientIp()
{
	return m_ip;
}

QString loginwindow::GetClientPort()
{
	return m_port;
}
QString loginwindow::GetClientName()
{
	return m_name;
}

bool loginwindow::GetStatus()
{
	return m_status;
}

std::shared_ptr<TCPClient> loginwindow::GetTCPClient()
{
	return m_client;
}

bool loginwindow::CheckOnValidInputData()
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

loginwindow::~loginwindow()
{
    delete ui;
}

void loginwindow::exit()
{
	m_ip = ui->ipLineEdit->text();
	m_port = ui->portLineEdit->text();
	m_name = ui->nameLineEdit->text();

	
	if (CheckOnValidInputData())
	{
		m_client.reset();
		m_client = nullptr;

		m_client = std::make_shared<TCPClient>(m_port.split(" ")[0].toInt(), m_ip.toUtf8().constData(), m_name.toUtf8().constData());

		if (m_client->Connect())
		{
			m_status = true;
			this->close();
		}
		else
		{
			m_status = false;
			if (m_client->isSameName())
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
