#pragma once
#include <QObject>
#include <QLabel>
#include <QWidget>
#include <QMouseEvent>
#include <mutex>
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
	QSize GetSize() const;

	void SetBoundaries(QPoint topLeftBorder,QPoint bottomRightBorder);
	void Clear();
	void SetVisibleLabel(bool visibility);
public slots:
	void ChangedCondition(bool value);
private:
	QLabel *m_nativeLabel;
	QPoint m_lastPos;
	QPoint m_topLeftBorder;
	QPoint m_bottomRightBorder;
	bool m_moving = false;
	QPoint m_topLeftPointLabel;
	int m_width;
	int m_height;
	bool m_isStream = false;
	std::mutex m_mutex;
};