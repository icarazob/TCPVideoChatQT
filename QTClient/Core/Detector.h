#pragma once
#include <string>

namespace cv {
	class Mat;
}

class Detector
{
public:

	Detector(std::string path = "");

	virtual void Process(cv::Mat& frame);

	virtual void ProcessInternal(cv::Mat& frame)
	{

	}

	virtual ~Detector()
	{

	}
protected:
	std::string m_appPath;
};
