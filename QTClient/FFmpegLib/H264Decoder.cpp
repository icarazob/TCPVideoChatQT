#include "H264Decoder.h"

H264Decoder::H264Decoder()
{
	av_register_all();

	Inititalize();
}

bool H264Decoder::Decode(uint8_t * data, int size)
{
	if (m_initialized)
	{
		AVPacket packet;
		av_init_packet(&packet);

		packet.data = data;
		packet.size = size;

		if (avcodec_send_packet(m_codecContext, &packet) < 0)
		{
			std::cout << "Can't send packet to context" << std::endl;
			av_packet_unref(&packet);
			return false;
		}

		AVFrame *avFrame = av_frame_alloc();
		int error;

		error = avcodec_receive_frame(m_codecContext, avFrame);

		if (error == AVERROR(EAGAIN) || error < 0)
		{
			std::cout << "Error" << std::endl;
			return false;
		}

		ConvertToMat(avFrame);

		av_frame_free(&avFrame);
		av_frame_unref(avFrame);
		av_packet_unref(&packet);

		return true;
	}
}

cv::Mat H264Decoder::GetFrame() const
{
	return m_currentFrame;
}


void H264Decoder::Inititalize()
{
	auto decoderCodec = avcodec_find_decoder(AV_CODEC_ID_H264);

	if (!decoderCodec)
	{
		std::cout << "Error find decoder";
		return;
	}

	m_codecContext = avcodec_alloc_context3(decoderCodec);

	if (!m_codecContext)
	{
		std::cout << "Error alloc decoder";
		return;
	}

	m_codecContext->width = 352;
	m_codecContext->height = 264;

	if (avcodec_open2(m_codecContext, decoderCodec, NULL) < 0)
	{
		avcodec_free_context(&m_codecContext);
		std::cout << "Error open decoder";
		return;
	}

	m_swsContext = sws_getContext(m_codecContext->width, m_codecContext->height,
		AV_PIX_FMT_YUV420P, m_codecContext->width, m_codecContext->height,
		AV_PIX_FMT_BGR24, 0, 0, 0, 0);

	m_initialized = true;
}

void H264Decoder::ConvertToMat(AVFrame* image)
{
	uint8_t *prgb24 = new uint8_t[3 * m_codecContext->width*m_codecContext->height];
	uint8_t *rgb24[1] = { prgb24 };

	int rgb24_stride[1] = { 3 * m_codecContext->width };

	sws_scale(m_swsContext, image->data, image->linesize, 0, image->height, rgb24, rgb24_stride);

	cv::Mat mat(m_codecContext->height, m_codecContext->width, CV_8UC3, rgb24[0], rgb24_stride[0]);
	mat.copyTo(m_currentFrame);

	delete []prgb24; //[] inserted

	return;
}

H264Decoder::~H264Decoder()
{
	if (m_codecContext)
	{
		avcodec_free_context(&m_codecContext);
	}

	if (m_swsContext)
	{
		sws_freeContext(m_swsContext);
	}
}
