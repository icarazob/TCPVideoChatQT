#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QMessageBox>

MainWindow::MainWindow(QString port, QString ip, QString name) :
    QMainWindow(0),
	m_port(port),
	m_ip(ip),
	m_name(name),
    ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	m_client = new TCPClient(m_port.split(" ")[0].toInt(), m_ip.toUtf8().constData(),m_name.toUtf8().constData());

	if (!m_client->Connect())
	{
		exit();
	}

	ui->nameLabel->setText(name);
	QObject::connect(m_client, SIGNAL(recieveEvent(QString)), this, SLOT(UpdatePlainText(QString)));
	QObject::connect(ui->buttonExit, SIGNAL(clicked()), SLOT(exit()));
	QObject::connect(ui->sendButton, SIGNAL(clicked()), SLOT(UpdatePlain()));

/*	QObject::connect(ui->actionLogin, SIGNAL(triggered()), SLOT(ShowLoginWindow()));*/

}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::UpdatePlain()
{
	QString lineText = ui->plainTextForSend->toPlainText();

	if (lineText.size() != 0)
	{
		//Send message
		m_client->SendMessage(lineText.toUtf8().constData());

		//Update plainEdit
		QString you("You");
		ui->plainTextEdit->appendPlainText(you + ": "+lineText);

		//clear plainTextForSend
		ui->plainTextForSend->clear();
	}
}
void MainWindow::UpdatePlainText(QString message)
{
	ui->plainTextEdit->appendPlainText(message);
}

void MainWindow::exit()
{
	this->close();
}