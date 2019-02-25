#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QWidget>
#include <QRadioButton>

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QWidget
{
	Q_OBJECT
private slots:

	void HaarCasscadeSlot();
	void HoGSlot();
	void FaceLandmarkSlot();
	void GroupBoxClicked(bool state);
	void SetIcon();

public:
    explicit SettingsWindow(QWidget *parent = 0);

	void SetAppPath(QString path);

    ~SettingsWindow();

Q_SIGNALS:

	void HaarCascadeCheckSignal();
	void HoGCheckSignal();
	void FaceLandmarkCheckSignal();
	void CloseDetectorSignal();

private:
    Ui::SettingsWindow *ui;
	QRadioButton *m_lastButton = nullptr;

	QString m_appPath;
};

#endif // SETTINGSWINDOW_H
