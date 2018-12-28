#include "H264Encoder.h"

AVFrame * H264Encoder::CreateAVFrameFromMat(SwsContext ** swsContext, AVCodecContext ** outputCodecContext, const cv::Mat & image)
{
	auto cctx = *outputCodecContext;
	AVFrame* frame = nullptr;

	frame = av_frame_alloc();

	frame->format = AV_PIX_FMT_YUV420P;
	frame->width = cctx->width;
	frame->height = cctx->height;

	av_frame_get_buffer(frame, 32);

	if (!*swsContext) {
		*swsContext = (sws_getContext(cctx->width, cctx->height, AV_PIX_FMT_BGR24,
			cctx->width, cctx->height, AV_PIX_FMT_YUV420P, 0, nullptr, nullptr, nullptr));
	}

	int inLinesize[1] = { 3 * cctx->width };

	int srcStride[4] = { image.cols * 3, 0, 0, 0 };
	uint8_t* srcSlice[1] = {image.data};

	sws_scale(*swsContext, srcSlice, srcStride, 0, image.rows, frame->data, frame->linesize);
	frame->pts = m_frameCounter++;

	return frame;
}


H264Encoder::H264Encoder()
{
	av_register_all();

	auto codec = avcodec_find_encoder(AV_CODEC_ID_H264);

	if (!codec)
	{
		std::cout << "Can't find codec" << std::endl;
	}


	m_codec_context = avcodec_alloc_context3(codec);

	if (!m_codec_context)
	{  
		std::cout << "Can't alloc context" << std::endl;
	}

	m_codec_context->time_base = { 1,20 };
	m_codec_context->max_b_frames = 2;
	m_codec_context->gop_size = 5;
	m_codec_context->pix_fmt = AV_PIX_FMT_YUV420P;
	m_codec_context->width = 640;
	m_codec_context->height = 480;

	av_opt_set(m_codec_context->priv_data, "preset", "ultrafast", 0);

	if (avcodec_open2(m_codec_context, codec, NULL) < 0)
	{
		avcodec_free_context(&m_codec_context);
		std::cout << "Can't open codec" << std::endl;
	}
}


bool H264Encoder::Encode(const cv::Mat & frame)
{
	int result = -1;

	AVFrame *m_avFrame = CreateAVFrameFromMat(&m_swsContext, &m_codec_context, frame);

	AVPacket packet;
	av_init_packet(&packet);
	
	packet.data = NULL;
	packet.size = 0;

	int error;

	if (avcodec_send_frame(m_codec_context, m_avFrame) < 0)
	{
		avcodec_free_context(&m_codec_context);
		sws_freeContext(m_swsContext);
		av_frame_free(&m_avFrame);
		std::cout << "Can't send frame" << std::endl;
		return false;
	}

	error = avcodec_receive_packet(m_codec_context, &packet);

	if (error < 0)
	{
		std::cout << "Can't receive packet" << std::endl;
		av_frame_free(&m_avFrame);
		av_frame_unref(m_avFrame);
		av_packet_unref(&packet);
		return false;
	}
	else
	{
		m_currentData.clear();
		m_currentData.reserve(packet.size);
		m_currentData.insert(m_currentData.end(), packet.data,packet.data+packet.size);
		m_currentSize = packet.size;

		av_frame_free(&m_avFrame);
		av_frame_unref(m_avFrame);
		av_packet_unref(&packet);
		return true;
	}
	
}

std::vector<uint8_t> H264Encoder::GetData() const
{
	return m_currentData;
}

int H264Encoder::GetSize() const
{
	return m_currentSize;
}



H264Encoder::~H264Encoder()
{
}
