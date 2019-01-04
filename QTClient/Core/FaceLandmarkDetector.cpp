#include "FaceLandmarkDetector.h"



void FaceLandmarkDetector::DrawRect(cv::Mat& frame, cv::Rect rectangle, double scaleWidth, double scaleHeight)
{
	int x1 = rectangle.tl().x * scaleWidth;
	int y1 = rectangle.tl().y * scaleHeight;
	int x2 = rectangle.br().x * scaleWidth;
	int y2 = rectangle.br().y * scaleHeight;

	cv::rectangle(frame, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 255, 0), 2, 4);
}

FaceLandmarkDetector::FaceLandmarkDetector(std::string appPath) :
	Detector(appPath)
{
	Initialize();
}

void FaceLandmarkDetector::ProcessInternal(cv::Mat & frame)
{
	if (m_predictorLoaded)
	{
		int inWidth = 0;
		int frameHeight = frame.rows;
		int frameWidth = frame.cols;

		inWidth = (int)((frameWidth / (float)frameHeight) * m_inHeight);

		float scaleHeight = frameHeight / (float)m_inHeight;
		float scaleWidth = frameWidth / (float)inWidth;

		cv::Mat frameDlibHogSmall;
		cv::resize(frame, frameDlibHogSmall, cv::Size(inWidth, m_inHeight));

		dlib::cv_image<dlib::bgr_pixel> dlibImage(frameDlibHogSmall);
		std::vector<dlib::rectangle> faces = m_detector(dlibImage);

		for (auto &rect : faces)
		{
			//Draw rectangle
			cv::Rect rectangle(cv::Point(rect.left(), rect.top()), cv::Point(rect.right(), rect.bottom()));
			DrawRect(frame, rectangle, scaleWidth, scaleHeight);

			const auto shape = m_pose_model(dlibImage, rect);

			//Draw points
			const int count = shape.num_parts();

			for (int i = 0; i < count; ++i)
			{
				const auto point = shape.part(i);

				int x1 = point.x()*scaleWidth;
				int y1 = point.y()*scaleHeight;

				cv::circle(frame, cv::Point(x1, y1), 2, cv::Scalar(0, 0, 255), 2);
			}
		}
	}
}

FaceLandmarkDetector::~FaceLandmarkDetector()
{

}

void FaceLandmarkDetector::Initialize()
{
	m_detector = dlib::get_frontal_face_detector();
	dlib::deserialize(m_appPath + "/models/shape_predictor_68_face_landmarks.dat") >> m_pose_model;

	m_predictorLoaded = true;
}
