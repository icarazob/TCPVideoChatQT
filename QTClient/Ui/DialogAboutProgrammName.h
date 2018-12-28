#ifndef DIALOGABOUTPROGRAMMNAME_H
#define DIALOGABOUTPROGRAMMNAME_H
#include <QDialog>
#include <QWidget>
#include <QObject>

namespace Ui {
	class DialogAboutProgrammName;
}


class DialogAboutProgrammName: public QDialog
{
	Q_OBJECT
public:

	explicit DialogAboutProgrammName(QWidget *parent = 0);
	~DialogAboutProgrammName();

private:
	Ui::DialogAboutProgrammName *ui;
};



#endif // DIALOGABOUTPROGRAMMNAME_H