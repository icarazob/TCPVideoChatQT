#pragma once

namespace cv {
	class Mat;
}

class Detector
{
public:

	Detector();

	virtual void Process(cv::Mat& frame);

	virtual void ProcessInternal(cv::Mat& frame) = 0;

	virtual ~Detector()
	{

	}
private:

};
