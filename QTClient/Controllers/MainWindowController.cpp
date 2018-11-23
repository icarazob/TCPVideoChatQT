#include "MainWindowController.h"
#include "Core/TCPClient.h"
#include "Core/AudioProcessor.h"
#include "Core/InformationStrings.h"
#include "UI/mainwindow.h"
#include <opencv2/highgui/highgui.hpp>



MainWindowController::MainWindowController(QString path):
	m_view(nullptr),
	m_tcpClient(nullptr),
	m_appPath(path),
	m_audioProcesscor(std::make_unique<AudioProcessor>()),
	m_videoCapture(std::make_unique<cv::VideoCapture>(m_appPath.toStdString() + "/Face.avi"))
{

}

void MainWindowController::SetView(MainWindow * view)
{
	Q_ASSERT(view);
	Q_ASSERT(m_tcpClient);

	m_view = view;

	m_view->SetAppPath(m_appPath);
	m_tcpClient->SetAppPath(m_appPath);

	SetClientInformation(m_tcpClient->GetClientInformation());
	
	QObject::connect(m_tcpClient.get(), SIGNAL(stopShowVideo()), this, SLOT(ViewStopShowVideo()));
	QObject::connect(m_tcpClient.get(), SIGNAL(startShowVideo()), this, SLOT(ViewStartShowVideo()));
	QObject::connect(m_tcpClient.get(), SIGNAL(recieveEventMessage(QString)), this, SLOT(ViewUpdatePlainText(QString)));
	QObject::connect(m_tcpClient.get(), SIGNAL(recieveEventAudio(QByteArray, int)), SLOT(ProcessAudioData(QByteArray, int)));
	QObject::connect(m_tcpClient.get(), SIGNAL(updateList(QString)), SLOT(ViewUpdateList(QString)));
	QObject::connect(m_tcpClient.get(), SIGNAL(recieveEventFrame()), SLOT(ViewShowFrame()));
	QObject::connect(m_audioProcesscor.get(), SIGNAL(audioDataPrepare(QByteArray, int)), SLOT(SendAudioSlot(QByteArray, int)));

	QObject::connect(m_view, &MainWindow::SendMessageSignal, this, &MainWindowController::SendMessageSlot);
	QObject::connect(m_view, &MainWindow::TurnAudioSignal, this, &MainWindowController::TurnAudioSlot);
	QObject::connect(m_view, &MainWindow::SendInformationSignal, this, &MainWindowController::SendInformationSlot);
	QObject::connect(m_view, &MainWindow::TurnVideoSignal, this, &MainWindowController::TurnVideoSlot);

	SendInformationSlot(InformationStrings::Setup());
}

void MainWindowController::SetTCPClient(std::unique_ptr<TCPClient> client)
{
	m_tcpClient = std::move(client);
}

MainWindowController::~MainWindowController()
{
	if (m_tcpClient != nullptr)
	{
		m_tcpClient->StopThread();
		TurnVideoSlot(false);
		m_audioProcesscor->CloseInput();
		SendInformationSlot(InformationStrings::StopVideo());

		//To do
		m_tcpClient->SendMessageWithoutName(m_clientInformation.name + " is disconnected!");
	}
}

void MainWindowController::ViewUpdatePlainText(QString message)
{
	m_view->UpdatePlainText(message);
}

void MainWindowController::ProcessAudioData(QByteArray data, int length)
{
	m_audioProcesscor->ProcessData(data, length);
}

void MainWindowController::ViewUpdateList(QString listOfClients)
{
	m_view->UpdateList(listOfClients);
}

void MainWindowController::SendMessageSlot(QString message)
{
	m_tcpClient->SendMessage(message.toUtf8().constData());
}

void MainWindowController::SendAudioSlot(QByteArray buffer, int length)
{
	m_tcpClient->SendAudio(buffer, length);
}

void MainWindowController::ViewStartShowVideo()
{
	m_view->StartShowVideo();
}

void MainWindowController::TurnAudioSlot(bool state)
{
	if (state)
	{
		m_audioProcesscor->StartInput();
	}
	else
	{
		m_audioProcesscor->CloseInput();
	}
}

void MainWindowController::TurnVideoSlot(bool state)
{
	if (state)
	{
		SendInformationSlot(InformationStrings::StartVideo());

		std::string threadName("VideoThread");
		std::thread videoThread(GetVideoHandler());

		m_shouldReadFrame = true;

		m_threadMap[threadName] = std::move(videoThread);
		m_threadMap[threadName].detach();		//????
	}
	else
	{
		std::string threadName("VideoThread");
		ThreadMap::iterator it = m_threadMap.find(threadName);

		if (it != m_threadMap.end())
		{
			m_shouldReadFrame = false;

			//condition variable
			SuspendThread(it->second.native_handle());
			m_threadMap.erase(threadName);
		}
	}

	return;

}

void MainWindowController::ViewShowFrame()
{
	cv::Mat image = m_tcpClient->GetCurrentFrame();

	if (!image.empty())
	{
		m_view->ShowFrame(image);
	}
	else
	{
		m_view->StopShowVideo();
	}

	return;
}

void MainWindowController::SendFrameSlot(std::vector<uchar> data)
{
	m_tcpClient->SendFrame(data);
}

void MainWindowController::SendInformationSlot(QString message)
{
	m_tcpClient->SendInformationMessage(message.toUtf8().constData());
}

void MainWindowController::ViewStopShowVideo()
{
	m_view->StopShowVideo();
}

std::function<void(void)> MainWindowController::GetVideoHandler()
{
	return [this]()
		{
			QSize frameLabelSize = m_view->GetFrameLabelSize();
			QSize nativeLabelSize = m_view->GetNativeLabelSize();

			const int c_widthLabel = frameLabelSize.width();
			const int c_heightLabel = frameLabelSize.height();
			const int c_widthNativeLabel = nativeLabelSize.width();
			const int c_heightNativeLabel = nativeLabelSize.height();

			m_videoCapture->open(m_appPath.toStdString() + "/Face.avi");

			if (!m_videoCapture->isOpened())
			{
				return;
			}
			cv::Mat frame;

			while (true)
			{
				if (m_shouldReadFrame)
				{
					bool successReadFrame = m_videoCapture->read(frame);

					if (!successReadFrame)
					{
						break;
					}

					cv::Mat resizedFrame;
					cv::resize(frame, resizedFrame, cv::Size(c_widthLabel, c_heightLabel));

					//Compress to .png extension
					std::vector<uchar> data;
					CompressFrame(resizedFrame, data);
					SendFrameSlot(data);

					//Show on view
					cv::resize(frame, frame, cv::Size(c_widthNativeLabel, c_heightNativeLabel));
					cv::cvtColor(frame, frame, CV_BGR2RGB);
					m_view->ShowFrameOnNativeLabel(frame);

					std::this_thread::sleep_for(std::chrono::milliseconds(25));
				}
				else
				{
					break;
				}
			}

			m_videoCapture->release();
			m_view->ClearNativeFrameLabel();

			SendInformationSlot(InformationStrings::StopVideo());

			return;
		};
}

void MainWindowController::CompressFrame(const cv::Mat & frame, std::vector<uchar>& buffer)
{
	cv::imencode(".png", frame, buffer);
}

void MainWindowController::SetClientInformation(std::tuple<std::string, std::string, int> information)
{

	m_clientInformation.name = std::get<0>(information);
	m_clientInformation.ip = std::get<1>(information);
	m_clientInformation.port = std::get<2>(information);

	m_view->SetNameLabel(QString::fromStdString(m_clientInformation.name));

}
