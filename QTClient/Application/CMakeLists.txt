set(APPLICATION_NAME Application)

set(SOURCES
    main.cpp
)

include_directories(
	${OpenCV_INCLUDE_DIRS}
	${DLIB_DIRS}
)

add_executable(${APPLICATION_NAME} ${SOURCES})

target_link_libraries(${APPLICATION_NAME}
		PUBLIC
        Core
        Ui
		Controllers
		Qt5::Core
		Qt5::Gui
		Qt5::Widgets
		Qt5::Multimedia	
		${OpenCV_LIBS}
		${DLIB_LIBS}/dlib.lib
)

add_dependencies(
    ${APPLICATION_NAME}
    Ui
	Controllers
	Core
)