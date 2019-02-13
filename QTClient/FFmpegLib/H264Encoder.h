#pragma once
#include <cstdint>
#include <opencv2/opencv.hpp>
#include <vector>

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

class H264Encoder
{
private:

	AVFrame* CreateAVFrameFromMat(SwsContext **swsContext, AVCodecContext **outputCodecContext, const cv::Mat& image);

public:
	explicit H264Encoder();

	bool Encode(const cv::Mat& frame);

	std::vector<uint8_t> GetData() const;
	int GetSize() const;


	~H264Encoder();

private:
	int m_frameCounter = 0;
	AVCodecContext *m_codec_context = nullptr;
	SwsContext* m_swsContext = nullptr;
	std::vector<uint8_t> m_currentData;
	int m_currentSize = 0;
};