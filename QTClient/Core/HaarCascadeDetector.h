#pragma once 
#include "Detector.h"
#include <opencv2/opencv.hpp>
#include <string>



class HaarCascadeDetector: public Detector
{
private:
	void DrawRect(cv::Mat& frame, cv::Rect rectangle, double scaleWidth, double scaleHeight);
public:
	HaarCascadeDetector(std::string appPath);

	virtual void ProcessInternal(cv::Mat& frame) override;

	virtual ~HaarCascadeDetector();

private:
	std::string m_appPath;
	cv::CascadeClassifier m_faceCascade;
	bool m_cascadeLoaded = false;
	int m_inHeight = 300;
};
