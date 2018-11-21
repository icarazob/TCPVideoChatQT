#include "FaceDetector.h"
#include "dlib/opencv/cv_image.h"
#include "dlib/image_processing/frontal_face_detector.h"

FaceDetector::FaceDetector(std::string appPath):
	m_appPath(appPath),
	m_loadHOG(false)
{

}

void FaceDetector::DetectFace(cv::Mat& frame)
{
	static dlib::frontal_face_detector m_hogFaceDetector;
	
	if (!m_loadHOG)
	{
		m_hogFaceDetector  = dlib::get_frontal_face_detector();
		m_loadHOG = true;
	}

	int inHeight = 300, inWidth = 0;
	int frameHeight = frame.rows;
	int frameWidth = frame.cols;

	inWidth = (int)((frameWidth / (float)frameHeight) * inHeight);

	float scaleHeight = frameHeight / (float)inHeight;
	float scaleWidth = frameWidth / (float)inWidth;

	cv::Mat frameDlibHogSmall;
	cv::resize(frame, frameDlibHogSmall, cv::Size(inWidth, inHeight));

	dlib::cv_image<dlib::bgr_pixel> dlibIm(frameDlibHogSmall);

	std::vector<dlib::rectangle> faces = m_hogFaceDetector(dlibIm);

	for (size_t i = 0; i < faces.size(); i++)
	{
		int x1 = (int)(faces[i].left() * scaleWidth);
		int y1 = (int)(faces[i].top() * scaleHeight);
		int x2 = (int)(faces[i].right() * scaleWidth);
		int y2 = (int)(faces[i].bottom() * scaleHeight);
		cv::rectangle(frame, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 255, 0), (int)(frameHeight / 150.0), 4);
	}

	return;
}

FaceDetector::~FaceDetector()
{
}
