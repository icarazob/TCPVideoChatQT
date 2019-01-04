#include "NativeFrameLabel.h"
#include <QEvent>

bool NativeFrameLabel::eventFilter(QObject * watched, QEvent * event)
{
	if (m_isStream)
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
				else if ((newX + m_width) > m_bottomRightBorder.x())
				{
					m_topLeftPointLabel.setX(m_bottomRightBorder.x() - m_width);
				}
				else /*(newX > m_topLeftBorder.x() && (newX + m_width) < m_bottomRightBorder.x())*/
				{
					m_topLeftPointLabel.setX(newX);
				}


				if(newY < m_topLeftBorder.y())
				{
					m_topLeftPointLabel.setY(m_topLeftBorder.y());
				}
				else if ((newY + m_height) > m_bottomRightBorder.y())
				{
					m_topLeftPointLabel.setY(m_bottomRightBorder.y() - m_height);
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
	else
	{
		return QObject::eventFilter(watched, event);
	}

}

NativeFrameLabel::NativeFrameLabel(QWidget *parent,QPoint point) :
	QWidget(nullptr)
{
	m_nativeLabel = new QLabel(parent);

	m_nativeLabel->setObjectName(QStringLiteral("nativeLabel"));
	m_nativeLabel->setGeometry(QRect(point.x() - 131, point.y()-131, 131, 131));
	m_nativeLabel->setText(QStringLiteral(""));
	m_nativeLabel->setAutoFillBackground(false);
	m_nativeLabel->setWordWrap(false);
	m_nativeLabel->setVisible(false);
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

void NativeFrameLabel::SetFrame(const cv::Mat& frame)
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

QSize NativeFrameLabel::GetSize() const
{
	return m_nativeLabel->size();
}

void NativeFrameLabel::SetBoundaries(QPoint topLeftBorder, QPoint bottomRightBorder)
{
	m_topLeftBorder = topLeftBorder;
	m_bottomRightBorder = bottomRightBorder;
}

void NativeFrameLabel::Clear()
{
	m_nativeLabel->clear();
	m_nativeLabel->setVisible(false);
}

void NativeFrameLabel::SetVisibleLabel(bool visibility)
{
	m_nativeLabel->setVisible(visibility);
}

void NativeFrameLabel::ChangedCondition(bool value)
{
	m_isStream = value;
}