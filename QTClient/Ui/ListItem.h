#ifndef LISTITEM_H
#define LISTITEM_H
#include <QWidget>
#include <QTextEdit>

class ListItem : public QWidget {

	Q_OBJECT

public:
	ListItem(QWidget *parent = nullptr);

	void SetText(QString text, bool isLeft);

	~ListItem();

private:
	QTextEdit *m_plainTextEdit;
};



#endif // LISTITEM_H