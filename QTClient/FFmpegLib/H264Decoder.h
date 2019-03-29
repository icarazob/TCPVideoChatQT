#pragma once
#include <cstdint>
#include <opencv2/opencv.hpp>

extern "C"
{
#include "libavformat/avformat.h"
#include "libavformat/avio.h"

#include "libavcodec/avcodec.h"
#include "libavutil/frame.h"
#include "libavutil/imgutils.h"

#include "libavutil/audio_fifo.h"
#include "libavutil/avassert.h"
#include "libavutil/avstring.h"
#include "libavutil/opt.h"
#include <libswscale/swscale.h>
#include "libswresample/swresample.h"

};


class H264Decoder {
private:
	void Inititalize();
	void ConvertToMat(AVFrame* image);
public:
	explicit H264Decoder();

	bool Decode(uint8_t* data, int size);

	cv::Mat GetFrame() const;
	
	~H264Decoder();
private:
	AVCodecContext* m_codecContext = nullptr;
	SwsContext* m_swsContext = nullptr;
	cv::Mat m_currentFrame;
	bool m_initialized = false;
};