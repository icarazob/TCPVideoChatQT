set(CORE_MODULE_NAME Core)

set(HEADERS
    AudioProcessor.h
    NativeFrameLabel.h
    TCPClient.h
    InformationStrings.h
)

set(SOURCES
    AudioProcessor.cpp
    NativeFrameLabel.cpp
    TCPClient.cpp
)

add_library(
    ${CORE_MODULE_NAME}
    STATIC
    ${HEADERS}
    ${SOURCES}
)

target_link_libraries(
    ${CORE_MODULE_NAME}
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Multimedia
	${OpenCV_LIBS}
)