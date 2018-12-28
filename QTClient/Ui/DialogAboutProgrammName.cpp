#include "DialogAboutProgrammName.h"
#include "ui_DialogAboutProgrammName.h"

DialogAboutProgrammName::DialogAboutProgrammName(QWidget *parent):
	QDialog(parent),
	ui(new Ui::DialogAboutProgrammName)
{
	ui->setupUi(this);
}

DialogAboutProgrammName::~DialogAboutProgrammName()
{
}
