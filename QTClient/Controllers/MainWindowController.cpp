#include "MainWindowController.h"
#include "SettingsWindowContoller.h"
#include "Core/TCPClient.h"
#include "Core/AudioProcessor.h"
#include "Core/InformationStrings.h"
#include "FFmpegLib/H264Encoder.h"
#include "UI/mainwindow.h"
#include "Ui/DialogAboutProgrammName.h"
#include "UI/settingswindow.h"
#include <opencv2/highgui/highgui.hpp>


#define __STDC_CONSTANT_MACROS
#define __STDC_FORMAT_MACROS



MainWindowController::MainWindowController(QString path) :
	m_view(nullptr),
	m_tcpClient(nullptr),
	m_appPath(path),
	m_audioProcesscor(std::make_unique<AudioProcessor>()),
	m_videoCapture(std::make_unique<cv::VideoCapture>())
{
	m_settingsView = new SettingsWindow();
	m_settingsWindowController = std::make_unique<SettingsWindowController>(path);
}

void MainWindowController::SetView(MainWindow * view)
{
	Q_ASSERT(view);
	Q_ASSERT(m_tcpClient);

	Initialize(view);
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
	QObject::connect(m_view, &MainWindow::AboutClickedSignal, this, &MainWindowController::ShowAboutWidget);
	QObject::connect(m_view, &MainWindow::SettingsClickedSignal, this, &MainWindowController::ShowSettingsWidget);

	QObject::connect(m_settingsWindowController.get(), &SettingsWindowController::ChangeDetectorSignal, this, &MainWindowController::ChangeDetectorSlot);
	QObject::connect(m_settingsWindowController.get(), &SettingsWindowController::CloseDetectorSignal, this, &MainWindowController::CloseDetectorSlot);

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

		ResetEncoder();

		cv::Mat frame;

		while (m_shouldReadFrame)
		{
			bool successReadFrame = m_videoCapture->read(frame);

			if (!successReadFrame)
			{
				break;
			}

			cv::Mat resizedFrame;
			cv::resize(frame, resizedFrame, cv::Size(c_widthLabel, c_heightLabel));

			if (m_encoder->Encode(resizedFrame))
			{
				//SendFrameSlot(CompressFrame(resizedFrame));
				SendFrame(m_encoder->GetData(), m_encoder->GetSize());

				//Show on view
				cv::resize(frame, frame, cv::Size(c_widthNativeLabel, c_heightNativeLabel));
				cv::cvtColor(frame, frame, CV_BGR2RGB);
				m_view->ShowFrameOnNativeLabel(frame);
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(25));
		}

		m_videoCapture->release();
		m_view->ClearNativeFrameLabel();
		m_view->StopVideoStream(); //??
		SendInformationSlot(InformationStrings::StopVideo());

		return;
	};
}

std::vector<uchar> MainWindowController::CompressFrame(const cv::Mat& frame)
{
	std::vector<uchar> data;
	data.resize(frame.rows*frame.cols);
	cv::imencode(".png", frame, data);
	return data;
}

void MainWindowController::SetClientInformation(std::tuple<std::string, std::string, int> information)
{
	m_clientInformation.name = std::get<0>(information);
	m_clientInformation.ip = std::get<1>(information);
	m_clientInformation.port = std::get<2>(information);

	m_view->SetNameLabel(QString::fromStdString(m_clientInformation.name));
}

void MainWindowController::SendFrame(std::vector<uint8_t> data, int size)
{
	m_tcpClient->SendFrame(data, size);
}

void MainWindowController::ResetEncoder()
{
	m_encoder.reset(nullptr);
	m_encoder = std::make_unique<H264Encoder>();
}

void MainWindowController::ShowAboutWidget()
{
	m_aboutView->show();
	m_aboutView->adjustSize();
}

void MainWindowController::ShowSettingsWidget()
{
	m_settingsWindowController->SetView(m_settingsView);

	m_settingsView->show();
	m_settingsView->adjustSize();
}

void MainWindowController::Initialize(MainWindow * view)
{
	m_view = view;

	m_view->SetAppPath(m_appPath);
	m_tcpClient->SetAppPath(m_appPath);

	m_aboutView = std::make_unique<DialogAboutProgrammName>(m_view);
}

void MainWindowController::ChangeDetectorSlot(int type)
{
	m_tcpClient->ChangeDetector(type);
}

void MainWindowController::CloseDetectorSlot()
{
	m_tcpClient->CloseDetector();
}
