set(CONTROLLERS_MODULE_NAME Controllers)

set(SOURCES
    MainController.cpp
    MainWindowController.cpp
)

set(HEADERS
    MainController.h
    MainWindowController.h
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
)