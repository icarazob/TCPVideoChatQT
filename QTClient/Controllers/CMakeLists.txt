set(CONTROLLERS_MODULE_NAME Controllers)

set(SOURCES
    MainController.cpp
    MainWindowController.cpp
)

set(HEADERS
    MainController.h
    MainWindowController.h
)

include_directories(
	${FFMPEG_DIRS}
)

add_library(
    ${CONTROLLERS_MODULE_NAME}
    STATIC
    ${SOURCES}
    ${HEADERS}
)

target_link_libraries(
    ${CONTROLLERS_MODULE_NAME}
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