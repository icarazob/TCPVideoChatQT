#pragma once
#include <opencv2/opencv.hpp>
#include <string>



class FaceDetector
{
public:
	FaceDetector(std::string appPath);

	void DetectFace(cv::Mat& frame);

	~FaceDetector();

private:
	bool m_loadHOG = false;
	std::string m_appPath;
};

