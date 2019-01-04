set(FFMPEG_MODULE_NAME FFmpegLib)

set(HEADERS
	H264Encoder.h
	H264Decoder.h
)

set(SOURCES
	H264Encoder.cpp
	H264Decoder.cpp
)

include_directories(
	${FFMPEG_DIRS}
)

add_library(
    ${FFMPEG_MODULE_NAME}
    STATIC
    ${HEADERS}
    ${SOURCES}
)

target_link_libraries(
    ${FFMPEG_MODULE_NAME}
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Multimedia
    ${OpenCV_LIBS}
	${FFMPEG_LIBS}/avcodec.lib
	${FFMPEG_LIBS}/avfilter.lib
	${FFMPEG_LIBS}/avformat.lib
	${FFMPEG_LIBS}/swscale.lib
	${FFMPEG_LIBS}/avutil.lib
)