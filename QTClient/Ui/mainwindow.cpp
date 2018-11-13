#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Core/NativeFrameLabel.h"
#include "PopUpNotification.h"
#include <QString>
#include <qpixmap.h>
#include <qimage.h>
#include <QLabel>
#include <QMessageBox>
#include <QKeyEvent>
#include <sstream>
#include <vector>
#include <string>
#include <QSound>
#include <opencv2\opencv.hpp>

MainWindow::MainWindow(QMainWindow *parent):
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	m_notification(std::make_unique<PopUpNotification>())
{

	ui->setupUi(this);
	
	QObject::connect(ui->buttonExit, SIGNAL(clicked()), SIGNAL(exit()));
	QObject::connect(ui->sendButton, SIGNAL(clicked()), SLOT(UpdatePlain()));
	QObject::connect(ui->videoButton, SIGNAL(clicked()), SLOT(StartVideoStream()));
	QObject::connect(ui->stopVideoButton, SIGNAL(clicked()), SLOT(StopVideoStream()));
	QObject::connect(ui->audioButton, SIGNAL(clicked()), SLOT(TurnAudio()));
	QObject::connect(ui->clientsList, SIGNAL(itemSelectionChanged()), this, SLOT(ListItemClicked()));

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

void MainWindow::SetNameLabel(std::string name)
{
	ui->nameLabel->setText(QString::fromStdString(name));
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
void MainWindow::ShowFrame(cv::Mat copyFrame)
{
	static const int c_widthLabel = ui->label->width();
	static const int c_heightLabel = ui->label->height();

	cv::Mat result;
	cv::resize(copyFrame, result, cv::Size(c_widthLabel, c_heightLabel));
	cv::cvtColor(result, result, CV_BGR2RGB);
	ui->label->setPixmap(QPixmap::fromImage(QImage(result.data, result.cols, result.rows, result.step, QImage::Format_RGB888)));

	return;
}

void MainWindow::ShowFrameOnNativeLabel(cv::Mat frame)
{
	m_nativeFrameLabel->SetFrame(frame);
}

void MainWindow::ClientStartVideo()
{
	ui->label->setVisible(true);
	ui->plainTextEdit->setGeometry(280, 390, 761, 221);
}

void MainWindow::UpdatePlain()
{
	QString lineText = ui->plainTextForSend->toPlainText();

	if (lineText.size() != 0)
	{
		//Send message
		SendMessageSignal(lineText);

		//Update plainEdit
		QString you("You");
		ui->plainTextEdit->appendPlainText(you + ": " + lineText);

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

	ClientStartVideo();
	
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
	ClearFrameLabel();
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


void MainWindow::ClearFrameLabel()
{
	ui->label->clear();
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

	icon.addFile(iconPath, QSize(), QIcon::Selected, QIcon::On);

	ui->audioButton->setIcon(icon);
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
	QIcon icon1, icon2, icon3;

	QString icon1ImagePath = m_path + "/images/Stop.png";
	QString icon2ImagePath = m_path + "/images/Play.png";
	QString icon3ImagePath = m_path + "/images/microphone.png";


	icon1.addFile(icon1ImagePath, QSize(), QIcon::Selected, QIcon::On);
	icon2.addFile(icon2ImagePath, QSize(), QIcon::Selected, QIcon::On);
	icon3.addFile(icon3ImagePath, QSize(), QIcon::Selected, QIcon::On);

	ui->stopVideoButton->setIcon(icon1);
	ui->videoButton->setIcon(icon2);
	ui->audioButton->setIcon(icon3);
}

void MainWindow::SetupElements()
{

	m_nativeFrameLabel = std::make_shared<NativeFrameLabel>(this);
	m_nativeFrameLabel->SetBoundaries(ui->label->geometry().topLeft(), ui->label->geometry().bottomRight());

	ui->plainTextForSend->installEventFilter(this);

	ui->label->setVisible(false);
}

void MainWindow::PlayNotificationSound(QString path)
{
	QSound::play(path);
}
