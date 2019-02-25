#include "settingswindow.h"
#include "ui_settingswindow.h"
#include <QFileInfo>


void SettingsWindow::HoGSlot()
{
	if (m_lastButton != ui->hogRadioButton)
	{
		m_lastButton = ui->hogRadioButton;
		HoGCheckSignal();
	}
}

void SettingsWindow::FaceLandmarkSlot()
{
	if (m_lastButton != ui->faceLandmarkRadioButton)
	{
		m_lastButton = ui->faceLandmarkRadioButton;
		FaceLandmarkCheckSignal();
	}
}

void SettingsWindow::HaarCasscadeSlot()
{
	if (m_lastButton != ui->haarRadioButton)
	{
		m_lastButton = ui->haarRadioButton;
		HaarCascadeCheckSignal();
	}
}

void SettingsWindow::GroupBoxClicked(bool state)
{
	if (!state)
	{
		ui->haarRadioButton->setAutoExclusive(false);
		ui->haarRadioButton->setChecked(false);
		ui->haarRadioButton->setAutoExclusive(true);

		ui->hogRadioButton->setAutoExclusive(false);
		ui->hogRadioButton->setChecked(false);
		ui->hogRadioButton->setAutoExclusive(true);

		ui->faceLandmarkRadioButton->setAutoExclusive(false);
		ui->faceLandmarkRadioButton->setChecked(false);
		ui->faceLandmarkRadioButton->setAutoExclusive(true);

		m_lastButton = nullptr;

		Q_EMIT CloseDetectorSignal();
	}
}

void SettingsWindow::SetIcon()
{
	QString windowIconPath = m_appPath + "/images/window-icon.png";

	QFileInfo checkFile(windowIconPath);

	if (checkFile.exists() && checkFile.isFile())
	{
		this->setWindowIcon(QIcon(windowIconPath));
	}
}

SettingsWindow::SettingsWindow(QWidget *parent) :
	QWidget(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);

	QObject::connect(ui->haarRadioButton, SIGNAL(clicked()), SLOT(HaarCasscadeSlot()));
	QObject::connect(ui->hogRadioButton, SIGNAL(clicked()),SLOT(HoGSlot()));
	QObject::connect(ui->faceLandmarkRadioButton, SIGNAL(clicked()), SLOT(FaceLandmarkSlot()));
	QObject::connect(ui->groupBox, SIGNAL(clicked(bool)), SLOT(GroupBoxClicked(bool)));
}

void SettingsWindow::SetAppPath(QString path)
{
	m_appPath = path;

	this->SetIcon();
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}
