#include "NativeFrameLabel.h"
#include <QEvent>
#include <QMessageBox>

bool NativeFrameLabel::eventFilter(QObject * watched, QEvent * event)
{
	if (event->type() == QEvent::MouseButtonPress)
	{
		QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
		if (mouseEvent->button() == Qt::LeftButton)
		{
			m_moving = true;
			m_lastPos = mouseEvent->pos();
			return true;
		}
		else
		{
			return QObject::eventFilter(watched, event);
		}
	}
	else if (event->type() == QEvent::MouseMove)
	{
		QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
		if (m_moving)
		{
			return true;
		}
		else
		{
			return QObject::eventFilter(watched, event);
		}
	}
	else if (event->type() == QEvent::MouseButtonRelease)
	{
		QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
		if (mouseEvent->button() == Qt::LeftButton)
		{

			QPoint currentPos = mouseEvent->pos();

			int biasX = currentPos.x() - m_lastPos.x();
			int biasY = currentPos.y() - m_lastPos.y();

			int newX = m_topLeftPointLabel.x() + biasX;
			int newY = m_topLeftPointLabel.y() + biasY;
			if (newX < m_topLeftBorder.x())
			{
				m_topLeftPointLabel.setX(m_topLeftBorder.x());
			}
			else
			{
				m_topLeftPointLabel.setX(newX);
			}
			if (newY < m_topLeftBorder.y())
			{
				m_topLeftPointLabel.setY(m_topLeftBorder.y());
			}
			else
			{
				m_topLeftPointLabel.setY(newY);
			}
			
			m_nativeLabel->setGeometry(m_topLeftPointLabel.x(), m_topLeftPointLabel.y(), m_width, m_height);

			m_moving = false;
			return true;
		}
		else
		{
			return QObject::eventFilter(watched, event);
		}

	}
	else
	{
		return QObject::eventFilter(watched, event);
	}
}

NativeFrameLabel::NativeFrameLabel(QWidget *parent) :
	QWidget(parent)
{
	m_nativeLabel = new QLabel(parent);

	m_nativeLabel->setObjectName(QStringLiteral("nativeLabel"));
	m_nativeLabel->setGeometry(QRect(730, 270, 131, 131));
	m_nativeLabel->setText(QStringLiteral(""));
	m_nativeLabel->setAutoFillBackground(false);
	m_nativeLabel->setWordWrap(false);

	m_nativeLabel->installEventFilter(this);


	QRect rectLabel = m_nativeLabel->geometry();
	m_width = rectLabel.width();
	m_height = rectLabel.height();
	m_topLeftPointLabel = rectLabel.topLeft();
}

NativeFrameLabel::~NativeFrameLabel()
{
	delete[] m_nativeLabel;

	this->close();
}

void NativeFrameLabel::SetFrame(cv::Mat frame)
{
	m_nativeLabel->setPixmap(QPixmap::fromImage(QImage(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888)));
	return;
}

int NativeFrameLabel::GetWidth()
{
	return m_width;
}

int NativeFrameLabel::GetHeight()
{
	return m_height;
}

void NativeFrameLabel::SetBoundaries(QPoint topLeftBorder, QPoint bottomRightBorder)
{
	m_topLeftBorder = topLeftBorder;
	m_bottomRightBorder = bottomRightBorder;
}
