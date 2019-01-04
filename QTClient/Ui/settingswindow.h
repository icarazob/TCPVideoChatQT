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

public:
    explicit SettingsWindow(QWidget *parent = 0);
    ~SettingsWindow();

Q_SIGNALS:

	void HaarCascadeCheckSignal();
	void HoGCheckSignal();
	void FaceLandmarkCheckSignal();
	void CloseDetectorSignal();

private:
    Ui::SettingsWindow *ui;
	QRadioButton *m_lastButton = nullptr;
};

#endif // SETTINGSWINDOW_H
