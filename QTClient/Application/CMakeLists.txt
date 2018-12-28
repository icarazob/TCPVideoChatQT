set(APPLICATION_NAME Application)

set(SOURCES
    main.cpp
)

include_directories(
	${OpenCV_INCLUDE_DIRS}
	${DLIB_DIRS}
	${FFMPEG_DIRS}
)

add_executable(${APPLICATION_NAME} ${SOURCES})


target_link_libraries(${APPLICATION_NAME}
		PUBLIC
        Core
        Ui
		Controllers
		FFmpegLib
		Qt5::Core
		Qt5::Gui
		Qt5::Widgets
		Qt5::Multimedia	
		${OpenCV_LIBS}
		${DLIB_LIBS}/dlib.lib
	    ${FFMPEG_LIBS}/avcodec.lib
	    ${FFMPEG_LIBS}/avfilter.lib
	    ${FFMPEG_LIBS}/avformat.lib
	    ${FFMPEG_LIBS}/swscale.lib
	    ${FFMPEG_LIBS}/avutil.lib 	
)

add_dependencies(
    ${APPLICATION_NAME}
    Ui
	Controllers
	Core
	FFmpegLib
)