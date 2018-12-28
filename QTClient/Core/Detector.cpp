#include "Detector.h"

Detector::Detector()
{

}

void Detector::Process(cv::Mat & frame)
{
	ProcessInternal(frame);
}
