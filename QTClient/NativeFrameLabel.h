#pragma once
#include <QObject>
#include <QLabel>
#include <QWidget>
#include <QMouseEvent>
#include <opencv2/opencv.hpp>

class NativeFrameLabel : public QWidget {
	Q_OBJECT
protected:
	bool eventFilter(QObject *watched, QEvent *event);
public:
	explicit NativeFrameLabel(QWidget *parent);
	~NativeFrameLabel();

	void SetFrame(cv::Mat frame);
	int GetWidth();
	int GetHeight();
	void SetBoundaries(QPoint topLeftBorder,QPoint bottomRightBorder);

private:
	QLabel *m_nativeLabel;
	QPoint m_lastPos;
	QPoint m_topLeftBorder;
	QPoint m_bottomRightBorder;
	bool m_moving = false;
	QPoint m_topLeftPointLabel;
	int m_width;
	int m_height;
};