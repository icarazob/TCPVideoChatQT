#include "ListItem.h"
#include <QLabel>
#include <QHBoxLayout>
#include <iostream>


ListItem::ListItem(QWidget *parent):
	QWidget(parent)
{

}

void ListItem::SetText(QString text, bool isLeft)
{
	m_plainTextEdit = new QTextEdit();
	m_plainTextEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_plainTextEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_plainTextEdit->setReadOnly(true);

	QFont font;
	QFontMetrics metric(font);

	auto length = metric.width(text);

	if (length < 200)
	{
		m_plainTextEdit->setFixedSize(QSize(length + 15, (metric.height() + 10)));
	}
	else
	{
		auto nRows = length / 200;

		m_plainTextEdit->setFixedSize(QSize(200, (metric.height() + 1) * (nRows + 1) + 10));
	}

	m_plainTextEdit->setPlainText(text);

	QHBoxLayout *mainLayout = new QHBoxLayout();

	if (isLeft)
	{
		mainLayout->addWidget(m_plainTextEdit);
		mainLayout->addStretch(1);

		m_plainTextEdit->setStyleSheet("border-radius: 5%;"
			"background-color: #F1F1F4;");
	}
	else
	{
		mainLayout->addStretch(1);
		mainLayout->addWidget(m_plainTextEdit);

		m_plainTextEdit->setStyleSheet("border-radius: 5%;"
			"background-color: #DEC79B ;");
	}

	mainLayout->setSpacing(0);

	this->setLayout(mainLayout);
	this->setFixedHeight(m_plainTextEdit->height()+5);
}

ListItem::~ListItem()
{

}
