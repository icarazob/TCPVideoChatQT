#pragma once
#include <QObject>

class SettingsWindow;



class SettingsWindowController :public QObject
{
	Q_OBJECT
public:

	explicit SettingsWindowController(QString path);

	void SetView(SettingsWindow* view);

	~SettingsWindowController();
Q_SIGNALS:

	void ChangeDetectorSignal(int type);
	void CloseDetectorSignal();

private slots:

	void HoGSelectedSlot();
	void HaarCascadeSelectedSlot();
	void FaceLandmarkSelectedSlot();

private:

	SettingsWindow *m_view;

	QString m_appPath;
};
