#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <qpixmap.h>
#include <qimage.h>
#include <opencv2\opencv.hpp>
#include <thread>

MainWindow::MainWindow(QString port, QString ip, QString name) :
    QMainWindow(0),
	m_port(port),
	m_ip(ip),
	m_name(name),
    ui(new Ui::MainWindow)
{
	ui->setupUi(this);


	m_client = std::make_shared<TCPClient>(m_port.split(" ")[0].toInt(), m_ip.toUtf8().constData(),m_name.toUtf8().constData());

	QObject::connect(m_client.get(), SIGNAL(recieveEvent(QString)), this, SLOT(UpdatePlainText(QString)));


	if (!m_client->Connect())
	{
		exit();
	}

	std::thread videoThread(GetVideoHandler());
	videoThread.detach();



	ui->nameLabel->setText(name);
	QObject::connect(ui->buttonExit, SIGNAL(clicked()), SLOT(exit()));
	QObject::connect(ui->sendButton, SIGNAL(clicked()), SLOT(UpdatePlain()));

	/*	QObject::connect(ui->, SIGNAL(returnPressed()), SLOT(UpdatePlain()));*/
/*	QObject::connect(ui->actionLogin, SIGNAL(triggered()), SLOT(ShowLoginWindow()));*/

}

MainWindow::~MainWindow()
{
    delete ui;
}

std::function<void (void)> MainWindow::GetVideoHandler()
{

	return [this]()
	{
		const int c_widthLabel = ui->label->width();
		const int c_heightLabel = ui->label->height();

		cv::VideoCapture capture(0);
		cv::Mat frame;

		if (!capture.isOpened())
		{
			std::cerr << "Video can't start! " << GetLastError() << std::endl;
			return;
		}
		int count = 0;
		while (true)
		{
			bool successReadFrame = capture.read(frame);

			if (!successReadFrame)
			{
				break;
			}

			cv::resize(frame, frame, cv::Size(c_widthLabel, c_heightLabel));
			m_client->SendFrame(frame);
			cv::cvtColor(frame, frame,CV_BGR2RGB);
			ui->label->setPixmap(QPixmap::fromImage(QImage(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888)));
			char c = cv::waitKey(25);

			if (c == 27)
			{
				break;
			}

			count++;
		}

		capture.release();
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
	this->close();
}