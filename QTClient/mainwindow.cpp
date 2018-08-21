#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	connect(ui->buttonExit, SIGNAL(clicked()), SLOT(exit()));
	connect(ui->sendButton, SIGNAL(clicked()), SLOT(SendMessage()));
	connect(ui->lineEdit, SIGNAL(returnPressed()), SLOT(SendMessage()));
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::SendMessage()
{
	QString lineText = ui->lineEdit->text();

	if (lineText.size() != 0)
	{
		QMessageBox *box = new QMessageBox;
		box->setWindowTitle("Box");
		box->setText(lineText);

		box->show();
	}
}

void MainWindow::exit()
{
	this->close();
}