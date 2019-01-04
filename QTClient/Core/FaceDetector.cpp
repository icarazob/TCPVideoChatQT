#include "FaceDetector.h"
#include <opencv2/opencv.hpp>

FaceDetector::FaceDetector(std::string appPath):
	Detector(appPath),
	m_loadHOG(false)
{
	Initialize();
}

void FaceDetector::ProcessInternal(cv::Mat& frame)
{
	if (m_loadHOG)
	{
		int inWidth = 0;
		int frameHeight = frame.rows;
		int frameWidth = frame.cols;

		inWidth = (int)((frameWidth / (float)frameHeight) * m_inHeight);

		float scaleHeight = frameHeight / (float)m_inHeight;
		float scaleWidth = frameWidth / (float)inWidth;

		cv::Mat frameDlibHogSmall;
		cv::resize(frame, frameDlibHogSmall, cv::Size(inWidth, m_inHeight));

		dlib::cv_image<dlib::bgr_pixel> dlibIm(frameDlibHogSmall);

		std::vector<dlib::rectangle> faces = m_hogFaceDetector(dlibIm);

		for (auto &face : faces)
		{
			int x1 = (int)(face.left() * scaleWidth);
			int y1 = (int)(face.top() * scaleHeight);
			int x2 = (int)(face.right() * scaleWidth);
			int y2 = (int)(face.bottom() * scaleHeight);
			cv::rectangle(frame, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 255, 0), (int)(frameHeight / 150.0), 4);
		}
	}

	return;
}

FaceDetector::~FaceDetector()
{
}

void FaceDetector::Initialize()
{
	m_hogFaceDetector = dlib::get_frontal_face_detector();
	m_loadHOG = true;
}
