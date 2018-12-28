#include "HaarCascadeDetector.h"
#include <vector>

void HaarCascadeDetector::DrawRect(cv::Mat & frame, cv::Rect rectangle, double scaleWidth, double scaleHeight)
{
	int x1 = (int)(rectangle.x * scaleWidth);
	int y1 = (int)(rectangle.y * scaleHeight);
	int x2 = (int)((rectangle.x + rectangle.width)*scaleWidth);
	int y2 = (int)((rectangle.y + rectangle.height)*scaleHeight);

	cv::rectangle(frame, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 255, 0), 2, 4);
}

HaarCascadeDetector::HaarCascadeDetector(std::string appPath) :
	m_appPath(appPath)
{
	if (m_faceCascade.load(m_appPath + "/models/haarcascade_frontalface_default.xml"))
	{
		m_cascadeLoaded = true;
	}
}

void HaarCascadeDetector::ProcessInternal(cv::Mat & frame)
{
	if (m_cascadeLoaded)
	{
		int frameHeight = frame.rows;
		int frameWidth = frame.cols;
		int inWidth = (int)((frameWidth / (float)frameHeight)*m_inHeight);

		float scaleHeight = frameHeight / (float)m_inHeight;
		float scaleWidth = frameWidth / float(inWidth);

		cv::Mat smallFrame, graySmallFrame;
		cv::resize(frame, smallFrame, cv::Size(inWidth, m_inHeight));
		cv::cvtColor(smallFrame, graySmallFrame, cv::COLOR_BGR2GRAY);

		std::vector<cv::Rect> faces;
		m_faceCascade.detectMultiScale(graySmallFrame, faces, 1.3, 7);

		for (size_t i = 0; i < faces.size(); ++i)
		{
			DrawRect(frame, faces[i], scaleWidth, scaleHeight);
		}
	}
}

HaarCascadeDetector::~HaarCascadeDetector()
{
}
