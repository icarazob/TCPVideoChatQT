#include "SettingsWindowContoller.h"
#include "UI/settingswindow.h"
#include "Common.h"



SettingsWindowController::SettingsWindowController(QString path) :
	m_view(nullptr),
	m_appPath(path)
{

}

void SettingsWindowController::SetView(SettingsWindow * view)
{
	Q_ASSERT(view);

	Initialize(view);

	QObject::connect(m_view, &SettingsWindow::HaarCascadeCheckSignal, this, &SettingsWindowController::HaarCascadeSelectedSlot);
	QObject::connect(m_view, &SettingsWindow::HoGCheckSignal, this, &SettingsWindowController::HoGSelectedSlot);
	QObject::connect(m_view, &SettingsWindow::FaceLandmarkCheckSignal, this, &SettingsWindowController::FaceLandmarkSelectedSlot);
	QObject::connect(m_view, &SettingsWindow::CloseDetectorSignal, this, &SettingsWindowController::CloseDetectorSignal);
}

SettingsWindowController::~SettingsWindowController()
{
	if (m_view)
	{
		delete m_view;
	}
}

void SettingsWindowController::HoGSelectedSlot()
{
	Q_EMIT ChangeDetectorSignal(int(DetectorType::HoG));
}

void SettingsWindowController::FaceLandmarkSelectedSlot()
{
	Q_EMIT ChangeDetectorSignal(int(DetectorType::FaceLandmark));
}

void SettingsWindowController::Initialize(SettingsWindow * settingsWindow)
{
	m_view = settingsWindow;

	m_view->SetAppPath(m_appPath);
}

void SettingsWindowController::HaarCascadeSelectedSlot()
{
	Q_EMIT ChangeDetectorSignal(int(DetectorType::Haar));
}
