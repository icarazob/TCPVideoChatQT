#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <qpixmap.h>
#include <qimage.h>
#include <QLabel>
#include <Windows.h>
#include <QMessageBox>
#include <QKeyEvent>
#include <QDebug>
#include <sstream>
#include <vector>
#include <string>
#include <QSound>



MainWindow::MainWindow(QString port, QString ip, QString name, std::shared_ptr<TCPClient> client) :
	QMainWindow(0),
	m_port(port),
	m_ip(ip),
	m_name(name),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	m_client = client;

	m_nativeFrameLabel = std::make_shared<NativeFrameLabel>(this);
	m_nativeFrameLabel->SetBoundaries(ui->label->geometry().topLeft(), ui->label->geometry().bottomRight());

	m_audio = std::make_shared<AudioProcessor>();

	ui->nameLabel->setText(name);
	ui->plainTextForSend->installEventFilter(this);

	//set icons
	QIcon icon1,icon2,icon3;
	QString appPath = QCoreApplication::applicationDirPath();
	QString icon1ImagePath = appPath + "/images/Stop.png";
	QString icon2ImagePath = appPath + "/images/Play.png";
	QString icon3ImagePath = appPath + "/images/microphone.png";
	m_path = appPath;

	icon1.addFile(icon1ImagePath, QSize(), QIcon::Selected, QIcon::On);
	icon2.addFile(icon2ImagePath, QSize(), QIcon::Selected, QIcon::On);
	icon3.addFile(icon3ImagePath, QSize(), QIcon::Selected, QIcon::On);

	ui->stopVideoButton->setIcon(icon1);
	ui->videoButton->setIcon(icon2);
	ui->audioButton->setIcon(icon3);


	//connects
	QObject::connect(ui->buttonExit, SIGNAL(clicked()), SLOT(exit()));
	QObject::connect(ui->sendButton, SIGNAL(clicked()), SLOT(UpdatePlain()));
	QObject::connect(ui->videoButton, SIGNAL(clicked()), SLOT(StartVideoStream()));
	QObject::connect(ui->stopVideoButton, SIGNAL(clicked()), SLOT(StopVideoStream()));
	QObject::connect(m_client.get(), SIGNAL(recieveEventFrame()), SLOT(ShowFrame()));
	QObject::connect(m_client.get(), SIGNAL(recieveEventMessage(QString)), this, SLOT(UpdatePlainText(QString)));
	QObject::connect(m_client.get(), SIGNAL(recieveEventAudio(QByteArray,int)), SLOT(ProcessAudioData(QByteArray,int)));
	QObject::connect(this, SIGNAL(videoStream(bool)), m_nativeFrameLabel.get(), SLOT(ChangedCondition(bool)));
	QObject::connect(ui->audioButton, SIGNAL(clicked()), SLOT(TurnAudio()));
	QObject::connect(m_audio.get(), SIGNAL(audioDataPreapre(QByteArray, int)), SLOT(SendAudio(QByteArray, int)));
	QObject::connect(m_client.get(), SIGNAL(clearLabel()), this, SLOT(ClearFrameLabel()));
	QObject::connect(m_client.get(), SIGNAL(updateList(QString)), SLOT(UpdateList(QString)));
	QObject::connect(ui->clientsList, SIGNAL(itemSelectionChanged()), this, SLOT(ListItemClicked()));


	m_client->SendInformationMessage("Setup");
}

MainWindow::~MainWindow()
{
	QString message = m_name + " is disconnected!";
	m_client->SendMessageWithoutName(message.toStdString());

	delete ui;
}
void MainWindow::ShowFrame()
{
	cv::Mat copyFrame(m_client->GetCurrentFrame());
	if (!copyFrame.empty())
	{
		cv::cvtColor(copyFrame, copyFrame, CV_BGR2RGB);
		ui->label->setPixmap(QPixmap::fromImage(QImage(copyFrame.data, copyFrame.cols, copyFrame.rows, copyFrame.step, QImage::Format_RGB888)));
	}
	else
	{
		ClearFrameLabel();
	}

	return;
}

std::function<void(void)> MainWindow::GetVideoHandler()
{
	return [this]()
	{
		const int c_widthLabel = ui->label->width();
		const int c_heightLabel = ui->label->height();
		const int c_widthNativeLabel = m_nativeFrameLabel->GetWidth();
		const int c_heightNativeLabel = m_nativeFrameLabel->GetHeight();


		m_capture.open(0);

		cv::Mat frame;

		if (!m_capture.isOpened())
		{
			qDebug() << "Can't open camera!";
			m_nativeFrameLabel->Clear();
			return;
		}

		while (true)
		{
			if(GetStatusVideoRead())
			{
				bool successReadFrame = m_capture.read(frame);

				if (!successReadFrame)
				{
					break;
				}

				cv::resize(frame, frame, cv::Size(c_widthLabel, c_heightLabel));
				m_client->SendFrame(frame);

				cv::resize(frame, frame, cv::Size(c_widthNativeLabel, c_heightNativeLabel));
				cv::cvtColor(frame, frame, CV_BGR2RGB);
				m_nativeFrameLabel->SetFrame(frame);

				cv::waitKey(25);
			}
			else
			{
				break;
			}
		}

		m_capture.release();
		m_nativeFrameLabel->Clear();

		m_client->SendInformationMessage("Stop Video");
		return;
	};

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
		ui->plainTextEdit->appendPlainText(you + ": " + lineText);

		//clear plainTextForSend
		ui->plainTextForSend->clear();
	}
}
void MainWindow::UpdatePlainText(QString message)
{
	//Alarm
	QString soundPath = m_path + "/sound/message.wav";
	QSound::play(soundPath);

	ui->plainTextEdit->appendPlainText(message);
}

void MainWindow::exit()
{
	this->close();
}

void MainWindow::StartVideoStream()
{
	ui->videoButton->setEnabled(false);
	ui->stopVideoButton->setEnabled(true);

	std::string threadName("VideoThread");
	std::thread videoThread(GetVideoHandler());

	SetStatusVideoRead(true);
	threadMap[threadName] = std::move(videoThread);
	threadMap[threadName].detach();		//????

	
	Q_EMIT videoStream(true);

}

void MainWindow::StopVideoStream()
{
	ui->videoButton->setEnabled(true);
	ui->stopVideoButton->setEnabled(false);

	std::string threadName("VideoThread");
	ThreadMap::iterator it = threadMap.find(threadName);

	if (it != threadMap.end())
	{
		SetStatusVideoRead(false);
		
		//condition variable
		SuspendThread(it->second.native_handle());
		threadMap.erase(threadName);

	}


	Q_EMIT videoStream(false);
	ClearFrameLabel();
	m_nativeFrameLabel->Clear();

	return;

}

void MainWindow::TurnAudio()
{
	if (!m_lastStateAudioButton)
	{
		m_audio->StartInput();
		ChangeMicrophoneIcon(true);
	}
	else
	{
		m_audio->CloseInput();
		ChangeMicrophoneIcon(false);
	}

	m_lastStateAudioButton = !m_lastStateAudioButton;

}

void MainWindow::SendAudio(QByteArray buffer, int length)
{
	m_client->SendAudio(buffer, length);
}

void MainWindow::ProcessAudioData(QByteArray data, int length)
{
	m_audio->ProcessData(data, length);
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
			if (stringItem.compare(m_name.toStdString()) != 0)
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
	GetHistoryWithClient(name.toStdString());

	//QMessageBox::information()
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

void MainWindow::SetStatusVideoRead(bool value)
{
	std::lock_guard<std::mutex> lock(m_videoMutex);
	m_shouldRead = value;
}

bool MainWindow::GetStatusVideoRead()
{
	std::lock_guard<std::mutex> lock(m_videoMutex);
	return m_shouldRead;
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
	m_client->SendInformationMessage("Save History");

	m_client->SendMessageWithoutName(clientName);

	//send ten messages(buffer)
	m_client->SendMessage("Igor");

	ClearPlainText();
}

void MainWindow::ClearPlainText()
{
	ui->plainTextEdit->clear();
}
