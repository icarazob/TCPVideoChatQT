#pragma once
#include "Detector.h"
#include <opencv2/opencv.hpp>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/opencv/cv_image.h>

class FaceLandmarkDetector: public Detector
{
private:

	void DrawRect(cv::Mat& frame, cv::Rect rectangle, double scaleWidth, double scaleHeight);

public:

	FaceLandmarkDetector(std::string appPath);

	void ProcessInternal(cv::Mat& frame) override;

	virtual ~FaceLandmarkDetector();

private:
	void Initialize();

	bool m_predictorLoaded = false;
	dlib::frontal_face_detector m_detector;
	dlib::shape_predictor m_pose_model;
	int m_inHeight = 350;
};
