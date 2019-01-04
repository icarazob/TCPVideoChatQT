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

	m_view = view;

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
	ChangeDetectorSignal(int(DetectorType::HoG));
}

void SettingsWindowController::FaceLandmarkSelectedSlot()
{
	ChangeDetectorSignal(int(DetectorType::FaceLandmark));
}

void SettingsWindowController::HaarCascadeSelectedSlot()
{
	ChangeDetectorSignal(int(DetectorType::Haar));
}
