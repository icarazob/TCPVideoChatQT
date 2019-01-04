#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Core/NativeFrameLabel.h"
#include "PopUpNotification.h"
#include <QString>
#include <qpixmap.h>
#include <qimage.h>
#include <QLabel>
#include <QKeyEvent>
#include <sstream>
#include <vector>
#include <string>
#include <QSound>
#include <QFileInfo>
#include <opencv2\opencv.hpp>

MainWindow::MainWindow(QMainWindow *parent):
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	m_notification(std::make_unique<PopUpNotification>()),
	m_stopShowVideo(true)
{
	ui->setupUi(this);
	
	QObject::connect(ui->sendButton, SIGNAL(clicked()), SLOT(UpdatePlain()));
	QObject::connect(ui->videoButton, SIGNAL(clicked()), SLOT(StartVideoStream()));
	QObject::connect(ui->stopVideoButton, SIGNAL(clicked()), SLOT(StopVideoStream()));
	QObject::connect(ui->audioButton, SIGNAL(clicked()), SLOT(TurnAudio()));
	QObject::connect(ui->clientsList, SIGNAL(itemSelectionChanged()),SLOT(ListItemClicked()));
	QObject::connect(ui->menuAbout, SIGNAL(aboutToShow()), SIGNAL(AboutClickedSignal()));
	QObject::connect(ui->settingsButton, SIGNAL(clicked()), SIGNAL(SettingsClickedSignal()));

	SetupElements();

	QObject::connect(this, SIGNAL(videoStream(bool)), m_nativeFrameLabel.get(), SLOT(ChangedCondition(bool)));
}

QSize MainWindow::GetFrameLabelSize() const
{
	return ui->label->size();
}

QSize MainWindow::GetNativeLabelSize() const
{
	return m_nativeFrameLabel->GetSize();
}

void MainWindow::SetNameLabel(QString name)
{
	ui->nameLabel->setText(name);
}

void MainWindow::SetAppPath(QString path)
{
	m_path = path;

	SetupIcons();
}

MainWindow::~MainWindow()
{
	delete ui;
}
void MainWindow::ShowFrame(const cv::Mat &copyFrame)
{
	if (!m_stopShowVideo)
	{
		cv::Mat result;
		//cv::resize(copyFrame, result, cv::Size(ui->label->width(), ui->label->height()));
		cv::cvtColor(copyFrame, result, CV_BGR2RGB);
		ui->label->setPixmap(QPixmap::fromImage(QImage(result.data, result.cols, result.rows, result.step, QImage::Format_RGB888)));
	}

	return;
}

void MainWindow::ShowFrameOnNativeLabel(const cv::Mat& frame)
{
	m_nativeFrameLabel->SetFrame(frame);
}

void MainWindow::StartShowVideo()
{
	m_stopShowVideo = false;
	ui->label->setVisible(true);
	ui->plainTextEdit->setGeometry(280, 480, 761, 200);
}

void MainWindow::UpdatePlain()
{
	QString lineText = ui->plainTextForSend->toPlainText();

	if (lineText.size() != 0)
	{
		//Send message
		SendMessageSignal(lineText);

		ui->plainTextEdit->appendPlainText("You: " + lineText);

		//clear plainTextForSend
		ui->plainTextForSend->clear();
	}
}
void MainWindow::UpdatePlainText(QString message)
{
	//Alarm
	PlayNotificationSound(m_path + "/sound/message.wav");

	//Show notification
	std::string nameOfClient, messageOfClient;
	std::string stdMessage = message.toStdString();
	auto pos = stdMessage.find_first_of(":");

	if (pos != std::string::npos)
	{
		nameOfClient = stdMessage.substr(0, pos);;
		messageOfClient = stdMessage.substr(pos + 1);
	}
	else
	{
		nameOfClient = "Server";
		messageOfClient = stdMessage;
	}

	m_notification->Show(QString::fromStdString(nameOfClient), QString::fromStdString(messageOfClient));

	//Add message
	ui->plainTextEdit->appendPlainText(message);
}

void MainWindow::StartVideoStream()
{
	ui->videoButton->setEnabled(false);
	ui->stopVideoButton->setEnabled(true);

	StartShowVideo();
	
	TurnVideoSignal(true);
	
	Q_EMIT videoStream(true);

	m_nativeFrameLabel->SetVisibleLabel(true);
}

void MainWindow::StopVideoStream()
{
	ui->videoButton->setEnabled(true);
	ui->stopVideoButton->setEnabled(false);

	TurnVideoSignal(false);

	Q_EMIT videoStream(false);

	m_nativeFrameLabel->Clear();

	return;
}

void MainWindow::TurnAudio()
{
	if (!m_lastStateAudioButton)
	{
		TurnAudioSignal(true);
		ChangeMicrophoneIcon(true);
	}
	else
	{
		TurnAudioSignal(false);
		ChangeMicrophoneIcon(false);
	}

	m_lastStateAudioButton = !m_lastStateAudioButton;
}


void MainWindow::StopShowVideo()
{
	m_stopShowVideo = true;
	ui->label->clear();
}
void MainWindow::ClearNativeFrameLabel()
{
	m_nativeFrameLabel->Clear();
}
void MainWindow::UpdateList(QString listOfClients)
{
	ui->clientsList->clear();

	std::string buf;
	std::stringstream ss(listOfClients.toStdString());

	std::vector<std::string> tokens;

	while(ss >> buf)
	{
		tokens.push_back(buf);
	}

	for (auto &stringItem: tokens)
	{
		if (!stringItem.empty())
		{
			if (stringItem.compare(ui->nameLabel->text().toStdString()) != 0)
			{
				QListWidgetItem *newItem = new QListWidgetItem();
				newItem->setText(QString::fromStdString(stringItem));

				int row = ui->clientsList->row(ui->clientsList->currentItem());
				ui->clientsList->insertItem(row, newItem);
			}
		}
	}

	if (ui->clientsList->count() == 1)
	{
		ui->clientsList->item(0)->setBackgroundColor(Qt::green);
	}

	return;
}
void MainWindow::ListItemClicked()
{

	static QListWidgetItem *previousItem = new QListWidgetItem();
	previousItem->setBackgroundColor(Qt::white);
	
	QListWidgetItem *newItem = new QListWidgetItem();
	newItem = ui->clientsList->currentItem();

	QString name = newItem->text();

	newItem->setBackgroundColor(Qt::green);

	previousItem = newItem;

	//Get History
	//GetHistoryWithClient(name.toStdString());

}
bool MainWindow::eventFilter(QObject * watched, QEvent * event)
{
	static bool enterPress = false;
	static bool enterRelease = true;
	static bool shiftPress = false;
	static bool shiftRelease = true;

	if (event->type() == QEvent::KeyPress) {

		QKeyEvent* key = static_cast<QKeyEvent*>(event);
		if ((key->key() == Qt::Key_Enter) || (key->key() == Qt::Key_Return)) {
			if (shiftRelease)
			{
				UpdatePlain();
			}
			else
			{
				ui->plainTextForSend->appendPlainText("\n");
			}

			return true;
		}
		else if (key->key() == Qt::Key_Shift)
		{
			shiftPress = true;
			shiftRelease = false;
		}
		else {

			return QObject::eventFilter(watched, event);
		}
	}
	else if (event->type() == QEvent::KeyRelease) {
		QKeyEvent* key = static_cast<QKeyEvent*>(event);

		if (key->key() == Qt::Key_Shift)
		{
			shiftPress = false;
			shiftRelease = true;
		}
		return QObject::eventFilter(watched, event);
	}

	return false;
}


void MainWindow::ChangeMicrophoneIcon(bool status)
{
	QIcon icon;
	QString iconPath;

	if (status)
	{
		iconPath = m_path + "/images/crossed_microphone.png";
	}
	else
	{
		iconPath = m_path + "/images/microphone.png";
	}

	if (FileExist(iconPath))
	{
		icon.addFile(iconPath, QSize(), QIcon::Selected, QIcon::On);
		ui->audioButton->setIcon(icon);
	}
	

	return;
}

void MainWindow::GetHistoryWithClient(std::string clientName)
{
	//m_client->SendInformationMessage("Save History");

	//m_client->SendMessageWithoutName(clientName);

	//send ten messages(buffer)
	//m_client->SendMessage("Igor");

	ClearPlainText();
}

void MainWindow::ClearPlainText()
{
	ui->plainTextEdit->clear();
}

void MainWindow::SetupIcons()
{
	QIcon icon1, icon2, icon3, icon4;

	QString icon1ImagePath = m_path + "/images/Stop.png";
	QString icon2ImagePath = m_path + "/images/Play.png";
	QString icon3ImagePath = m_path + "/images/microphone.png";
	QString icon4ImagePath = m_path + "/images/settings.png";

	if (FileExist(icon1ImagePath))
	{
		icon1.addFile(icon1ImagePath, QSize(), QIcon::Selected, QIcon::On);
		ui->stopVideoButton->setIcon(icon1);
	}

	if (FileExist(icon2ImagePath))
	{
		icon2.addFile(icon2ImagePath, QSize(), QIcon::Selected, QIcon::On);
		ui->videoButton->setIcon(icon2);
	}

	if (FileExist(icon3ImagePath))
	{
		icon3.addFile(icon3ImagePath, QSize(), QIcon::Selected, QIcon::On);
		ui->audioButton->setIcon(icon3);
	}

	if (FileExist(icon4ImagePath))
	{
		icon4.addFile(icon4ImagePath, QSize(), QIcon::Selected, QIcon::On);
		ui->settingsButton->setIcon(icon4);
	}

	return;
}

void MainWindow::SetupElements()
{
	m_nativeFrameLabel = std::make_shared<NativeFrameLabel>(this, ui->label->geometry().bottomRight());
	m_nativeFrameLabel->SetBoundaries(ui->label->geometry().topLeft(), ui->label->geometry().bottomRight());

	ui->plainTextForSend->installEventFilter(this);

	ui->label->setVisible(false);
}

void MainWindow::PlayNotificationSound(QString path)
{
	QSound::play(path);
}

bool MainWindow::FileExist(QString path)
{
	QFileInfo checkFile(path);

	if (checkFile.exists() && checkFile.isFile())
	{
		return true;
	}
	else
	{
		return false;
	}
}
