#ifndef POPUPNOTIFICATION_H
#define POPUPNOTIFICATION_H
#pragma once
#include <QWidget>
#include <QString>

namespace Ui {
class PopUpNotification;
}

class PopUpNotification : public QWidget
{
    Q_OBJECT
private:
	void SetFields(QString client, QString message);
	void SetGeometry();
public:
	explicit PopUpNotification();

	void Show(QString client, QString message);

    ~PopUpNotification();

private:
	Ui::PopUpNotification *ui;
	int m_PosX = 0;
	int m_PosY = 0;
};

#endif // POPUPNOTIFICATION_H
