#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Core/NativeFrameLabel.h"
#include "PopUpNotification.h"
#include "ListItem.h"
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
#include <QFileDialog>
#include <opencv2\opencv.hpp>
#include <QDebug>


MainWindow::MainWindow(QMainWindow *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	m_notification(std::make_unique<PopUpNotification>()),
	m_stopShowVideo(true),
	m_labelSize(352, 264)
{
	ui->setupUi(this);

	Initialize();

	QObject::connect(ui->videoButton, SIGNAL(clicked()), SLOT(StartVideoStream()));
	QObject::connect(ui->videoButton, SIGNAL(clicked()), SIGNAL(StartShowVideoSignal()));
	QObject::connect(ui->stopVideoButton, SIGNAL(clicked()), SLOT(StopVideoStream()));
	QObject::connect(ui->audioButton, SIGNAL(clicked()), SLOT(TurnAudio()));
	QObject::connect(ui->menuAbout, SIGNAL(aboutToShow()), SIGNAL(AboutClickedSignal()));
	QObject::connect(ui->settingsButton, SIGNAL(clicked()), SIGNAL(SettingsClickedSignal()));
	QObject::connect(ui->userImage, SIGNAL(clicked()), SLOT(SelectUserImage()));
	QObject::connect(this, SIGNAL(videoStream(bool)), m_nativeFrameLabel.get(), SLOT(ChangedCondition(bool)));
}

void MainWindow::Initialize()
{
	ui->listWidget->setSelectionMode(QAbstractItemView::NoSelection);
	ui->listWidget->setFocusPolicy(Qt::NoFocus);
	ui->listWidget->setSpacing(0);
	ui->listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui->listWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

	ui->listWidget->setVerticalScrollBar(this->GetScrollBar());
	ui->plainTextForSend->setVerticalScrollBar(this->GetScrollBar());

	this->SetupElements();
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

void MainWindow::SetImageLabel(const QChar character)
{
	ui->userImage->setText(character);
}

void MainWindow::SetAppPath(QString path)
{
	m_path = path;
	m_soundPath = m_path + "/sound/message.wav";
	m_defaultUserLogo = m_path + "//images/user_logo.png";

	this->SetupIcons();
	this->ChangeUserLogo();
}

void MainWindow::SetVisibleLabel(bool visibility)
{
	ui->label->clear();
	ui->label->setVisible(visibility);
}

void MainWindow::DeleteFrameLabels()
{
	for (auto &item : m_labels.toStdMap())
	{
		auto currentLabel = item.second;

		ui->labelsLayout->removeWidget(currentLabel);
		delete currentLabel;
	}

	m_labels.clear();
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
		cv::cvtColor(copyFrame, result, CV_BGR2RGB);
		cv::resize(result, result, cv::Size(ui->label->width(), ui->label->height()));
		ui->label->setPixmap(QPixmap::fromImage(QImage(result.data, result.cols, result.rows, result.step, QImage::Format_RGB888)));
	}

	return;
}

void MainWindow::ShowFrameMultipleMode(const cv::Mat & copyFrame, QString labelName)
{
	if (m_labels.contains(labelName))
	{
		if (!copyFrame.empty())
		{
			cv::Mat result;
			cv::cvtColor(copyFrame, result, CV_BGR2RGB);
			cv::resize(result, result, cv::Size(m_labelSize.width(), m_labelSize.height()));

			m_labels.value(labelName)->setPixmap(QPixmap::fromImage(QImage(result.data, result.cols, result.rows, result.step, QImage::Format_RGB888)));
		}
	}
	else
	{
		QLabel *currentLabel = new QLabel();
		currentLabel->setFixedSize(m_labelSize);

		ui->labelsLayout->addWidget(currentLabel);

		m_labels.insert(labelName, currentLabel);
	}
}

void MainWindow::ShowFrameOnNativeLabel(const cv::Mat& frame)
{
	m_nativeFrameLabel->SetFrame(frame);
}

void MainWindow::StartShowVideo()
{
	m_stopShowVideo = false;
	ui->listWidget->setMinimumHeight(150);
	ui->label->setVisible(true);

	this->UpdateNativeLabel();
	//ui->plainTextEdit->setGeometry(280, 480, 761, 200);
}

void MainWindow::UpdatePlain()
{
	QString lineText = ui->plainTextForSend->toPlainText();

	if (lineText.size() != 0)
	{
		//Send message
		SendMessageSignal(lineText);

		this->AddItemToList(lineText, false);

		//clear plainTextForSend
		ui->plainTextForSend->clear();
	}
}
void MainWindow::UpdatePlainText(QString message)
{
	//Alarm
	PlayNotificationSound(m_soundPath);

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

	//m_notification->Show(QString::fromStdString(nameOfClient), QString::fromStdString(messageOfClient));

	//Add message
	this->AddItemToList(message, true);
}

void MainWindow::StartVideoStream()
{
	ui->videoButton->setVisible(false);
	ui->stopVideoButton->setVisible(true);

	//StartShowVideo();
	
	TurnVideoSignal(true);
	
	Q_EMIT videoStream(true);

	m_nativeFrameLabel->SetVisibleLabel(true);
}

void MainWindow::StopVideoStream()
{
	ui->videoButton->setVisible(true);
	ui->stopVideoButton->setVisible(false);

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

void MainWindow::StopVideo()
{
	ui->videoButton->setVisible(true);
	ui->stopVideoButton->setVisible(false);

	Q_EMIT videoStream(false);
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
		ui->clientsList->item(0)->setBackgroundColor(QColor("#F1F1F4"));
	}

	return;
}

void MainWindow::ListItemClicked()
{
	static QListWidgetItem *previousItem = new QListWidgetItem();
	previousItem->setBackgroundColor(Qt::white);
	
	//twice initialize
	QListWidgetItem *newItem = new QListWidgetItem();
	newItem = ui->clientsList->currentItem();

	QString name = newItem->text();

	newItem->setBackgroundColor(Qt::green);

	previousItem = newItem;

	//Get History
	//GetHistoryWithClient(name.toStdString());
}

void MainWindow::SelectUserImage()
{
	QString filename = QFileDialog::getOpenFileName(this, 
		tr("Choose file"),
		QDir::currentPath(),
		tr("Images (*.png *.jpg);;All Files (*)"));

	if (!filename.isNull())
	{
		//open Image
		QImage userLogo;
		userLogo.load(filename);

		//save Image
		userLogo.save(m_defaultUserLogo);

		this->ChangeUserLogo();
	}
}

bool MainWindow::eventFilter(QObject * watched, QEvent * event)
{
	static bool enterPress = false;
	static bool enterRelease = true;
	static bool shiftPress = false;
	static bool shiftRelease = true;

	if(event->type() == QEvent::KeyPress) 
	{

		QKeyEvent* key = static_cast<QKeyEvent*>(event);
		if ((key->key() == Qt::Key_Enter) || (key->key() == Qt::Key_Return)) {
			if (shiftRelease)
			{
				UpdatePlain();
			}
			else
			{
				auto text = ui->plainTextForSend->toPlainText();
				ui->plainTextForSend->setPlainText(text + "\n");
				ui->plainTextForSend->moveCursor(QTextCursor::End);
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

void MainWindow::AddItemToList(QString text, bool isClientMessage)
{
	ListItem *item = new ListItem(this);
	item->SetText(text, isClientMessage);

	QListWidgetItem *itemList = new QListWidgetItem(ui->listWidget);
	itemList->setSizeHint(QSize(ui->listWidget->width()-15, item->height()));
	ui->listWidget->addItem(itemList);
	ui->listWidget->setItemWidget(itemList, item);
	
	//scroll to bottom
	ui->listWidget->scrollToBottom();
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
	//ui->plainTextEdit->clear();
}

void MainWindow::SetupIcons()
{
	QIcon icon1, icon2, icon3, icon4;

	QString icon1ImagePath = m_path + "/images/Stop.png";
	QString icon2ImagePath = m_path + "/images/Play.png";
	QString icon3ImagePath = m_path + "/images/microphone.png";
	QString icon4ImagePath = m_path + "/images/settings.png";
	QString windowIconPath = m_path + "/images/window-icon.png";

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

	if (FileExist(windowIconPath))
	{
		this->setWindowIcon(QIcon(windowIconPath));
	}

	return;
}

void MainWindow::SetupElements()
{
	m_userImageStylesheet = ui->userImage->styleSheet();
	ui->plainTextForSend->installEventFilter(this);
	
	m_nativeFrameLabel = std::make_shared<NativeFrameLabel>(this, ui->label->geometry().bottomRight());

	ui->label->setVisible(false);
	ui->stopVideoButton->setVisible(false);
}

void MainWindow::UpdateNativeLabel()
{
	m_nativeFrameLabel->SetPosition(ui->label->geometry().center());
	m_nativeFrameLabel->SetBoundaries(ui->label->geometry().topLeft(), ui->label->geometry().bottomRight());
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

void MainWindow::ChangeUserLogo()
{
	if (FileExist(m_defaultUserLogo))
	{
		auto userImageStylesheet = m_userImageStylesheet + "\nborder-image:" + "url(" + m_defaultUserLogo + ");";

		ui->userImage->setStyleSheet(userImageStylesheet);
		ui->userImage->setText("");
	}
	else
	{
		this->SetImageLabel(ui->nameLabel->text().at(0));
	}
}

void MainWindow::resizeEvent(QResizeEvent * event)
{
	int count = ui->listWidget->count();

	for (int i = 0; i < count; ++i)
	{
		auto *curItem = ui->listWidget->item(i);
		curItem->setSizeHint(QSize(ui->listWidget->width() - 15, curItem->sizeHint().height()));
	}

	this->UpdateNativeLabel();
}

QScrollBar* MainWindow::GetScrollBar()
{
	QScrollBar* scrollBar = new QScrollBar();
	scrollBar->setStyleSheet(
		"QScrollBar:vertical {"
		"background:white;"
		"width: 8px;"
		"padding-top: 5px;"
		"}"
		"QScrollBar::handle:vertical {"
		"background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
		"stop: 0 rgb(190, 190, 190), stop: 0.5 rgb(190, 190, 190), stop:1 rgb(190, 190, 190));"
		"min-height: 0px;"
		"border-top: 4px;"
		"border-bottom: 4px;"
		"border-radius: 4px;"
		"}"
		"QScrollBar::add-line:vertical {"
		"background: qlineargradient(x1:0, y1:0, x2:1, y2:0"
		"stop: 0 rgb(190, 190, 190), stop: 0.5 rgb(190, 190, 190), stop:1 rgb(190, 190, s190));"
		"height: 0px;"
		"subcontrol-position: bottom;"
		"subcontrol-origin: margin;"
		"}"
		"QScrollBar::sub-line:vertical {"
		"background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
		"stop: 0 rgb(190, 190, 190), stop: 0.5 rgb(190, 190, 190), stop:1 rgb(190, 190, 190));"
		"height: 0 px;"
		"subcontrol-position: top;"
		"subcontrol-origin: margin;"
		"}"
	);

	return scrollBar;
}
