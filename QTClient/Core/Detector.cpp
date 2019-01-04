#include "Detector.h"

Detector::Detector(std::string path) :
	m_appPath(path)
{

}

void Detector::Process(cv::Mat & frame)
{
	ProcessInternal(frame);
}

