#include "PopUpNotification.h"
#include "ui_PopUpNotification.h"
#include <QApplication>
#include <QDesktopWidget>

void PopUpNotification::SetGeometry()
{
	auto desktopRect = QApplication::desktop()->screenGeometry();

	m_PosX = desktopRect.width() - this->width();
	m_PosY = 20;
}

PopUpNotification::PopUpNotification() :
    QWidget(nullptr),
    ui(new Ui::PopUpNotification)
{
    ui->setupUi(this);

	setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowStaysOnTopHint | Qt::Popup);
	SetGeometry();

	QObject::connect(ui->pushButton,SIGNAL(clicked()), this, SLOT(close()));

}

void PopUpNotification::Show(QString client, QString message)
{
	this->SetFields(client, message);

	this->move(m_PosX, m_PosY);
	this->show();
}

PopUpNotification::~PopUpNotification()
{
    delete ui;
}

void PopUpNotification::SetFields(QString client, QString message)
{
	ui->nameLabel->setText(client);
	ui->plainText->setPlainText(message);
}