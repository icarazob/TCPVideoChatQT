#pragma once
#include "Detector.h"
#include <string>
#include "dlib/opencv/cv_image.h"
#include "dlib/image_processing/frontal_face_detector.h"


namespace cv {
	class Mat;
}

class FaceDetector: public Detector
{
public:
	FaceDetector(std::string appPath);

	void ProcessInternal(cv::Mat& frame) override;

	virtual ~FaceDetector();

private:
	void Initialize();

	bool m_loadHOG = false;
	dlib::frontal_face_detector m_hogFaceDetector;
	int m_inHeight = 350;
};

