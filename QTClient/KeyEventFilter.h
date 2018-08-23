#pragma once
#include <QObject>

class KeyEventFiler :public QObject {
	Q_OBJECT
protected:
	bool eventFilter(QObject *watched, QEvent *event);
public:
	KeyEventFiler();
	~KeyEventFiler();
};