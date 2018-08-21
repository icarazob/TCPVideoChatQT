#include "loginwindow.h"
#include "ui_loginwindow.h"
#include <QFile>
#include <QXmlStreamReader>
#include <QCoreApplication>

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
		this->close();
	}
	else
	{
		ui->okButton->setStyleSheet("background-color:red");
	}
}
