#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <qpixmap.h>
#include <qimage.h>
#include <Windows.h>


MainWindow::MainWindow(QString port, QString ip, QString name,std::shared_ptr<TCPClient> client) :
    QMainWindow(0),
	m_port(port),
	m_ip(ip),
	m_name(name),
    ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	m_client = client;

	//m_client = std::make_shared<TCPClient>(m_port.split(" ")[0].toInt(), m_ip.toUtf8().constData(),m_name.toUtf8().constData());

	QObject::connect(m_client.get(), SIGNAL(recieveEvent(QString)), this, SLOT(UpdatePlainText(QString)));


	if (!m_client->Connect())
	{
		exit();
	}

	ui->nameLabel->setText(name);
	QObject::connect(ui->buttonExit, SIGNAL(clicked()), SLOT(exit()));
	QObject::connect(ui->sendButton, SIGNAL(clicked()), SLOT(UpdatePlain()));
	QObject::connect(ui->videoButton, SIGNAL(clicked()), SLOT(StartVideoStream()));
	QObject::connect(ui->stopVideoButton, SIGNAL(clicked()), SLOT(StopVideoStream()));
	QObject::connect(m_client.get(), SIGNAL(recieveEventFrame()), SLOT(ShowFrame()));

	/*	QObject::connect(ui->, SIGNAL(returnPressed()), SLOT(UpdatePlain()));*/
/*	QObject::connect(ui->actionLogin, SIGNAL(triggered()), SLOT(ShowLoginWindow()));*/

}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::ShowFrame()
{
	cv::Mat copyFrame = m_client->GetCurrentFrame();
	if (!copyFrame.empty())
	{
		cv::cvtColor(copyFrame, copyFrame, CV_BGR2RGB);
		ui->label->setPixmap(QPixmap::fromImage(QImage(copyFrame.data, copyFrame.cols, copyFrame.rows, copyFrame.step, QImage::Format_RGB888)));
	}
	else
	{
		ui->label->clear();
	}

	return;
}
std::function<void (void)> MainWindow::GetVideoHandler()
{
	return [this]()
	{
		const int c_widthLabel = ui->label->width();
		const int c_heightLabel = ui->label->height();

		m_capture.open(0);
		cv::Mat frame;

		if (!m_capture.isOpened())
		{
			std::cerr << "Video can't start! " << GetLastError() << std::endl;
			ui->label->clear();
			return;
		}
		int count = 0;
		while (true)
		{
			bool successReadFrame = m_capture.read(frame);

			if (!successReadFrame)
			{
	
				break;
			}

			cv::resize(frame, frame, cv::Size(c_widthLabel, c_heightLabel));
			m_client->SendFrame(frame);

			char c = cv::waitKey(25);

			if (c == 27)
			{
				break;
			}

			count++;
		}

		ui->label->clear();
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
	if (m_capture.isOpened())
	{
		m_capture.release();
	}
	this->close();
}

void MainWindow::StartVideoStream()
{
	ui->videoButton->setEnabled(false);
	ui->stopVideoButton->setEnabled(true);

	std::string threadName("VideoThread");
	std::thread videoThread(GetVideoHandler());

	threadMap[threadName] = std::move(videoThread);
	threadMap[threadName].detach();

}

void MainWindow::StopVideoStream()
{
	ui->videoButton->setEnabled(true);
	ui->stopVideoButton->setEnabled(false);

	std::string threadName("VideoThread");
	ThreadMap::iterator it = threadMap.find(threadName);

	if (it != threadMap.end())
	{
		SuspendThread(it->second.native_handle());
		threadMap.erase(threadName);

		m_capture.release();

	}

	ui->label->clear();
	return;

}