SET(execName TCPServer)
CMAKE_MINIMUM_REQUIRED(VERSION 2.4)
PROJECT(${execName})

set(OpenCV_DIR "C:/opencv340/opencv/build")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

set(DB_FILE ${PROJECT_SOURCE_DIR}/database/chat.db)
set(XML_FILE ${PROJECT_SOURCE_DIR}/settings/settings.xml)

find_package(OpenCV REQUIRED PATHS ${OpenCV_DIR})
find_package(Qt5 REQUIRED COMPONENTS Core Sql Widgets)

foreach(CONFIGURATION ${CMAKE_CONFIGURATION_TYPES})
	file(COPY ${DB_FILE} DESTINATION ${EXECUTABLE_OUTPUT_PATH}/${CONFIGURATION}/database)
	file(COPY ${XML_FILE} DESTINATION ${EXECUTABLE_OUTPUT_PATH}/${CONFIGURATION}/settings)
endforeach()

SET (SOURCES 
	TCPServer.cpp
	main.cpp
)

SET (HEADERS
	TCPServer.h
)


include_directories( ${OpenCV_INCLUDE_DIRS})

add_subdirectory(DbLib)

ADD_EXECUTABLE(${execName} ${SOURCES} ${HEADERS})

target_link_libraries(${execName}
		PUBLIC
		Qt5::Core
		Qt5::Sql
		Qt5::Widgets
		DbLib
		${OpenCV_LIBS}
	)
add_definitions(-std=c++11 -g -O3)


#TARGET_LINK_LIBRARIES(${execName}
 #                       ${ZED_LIBRARIES}
  #                      ${CUDA_CUDA_LIBRARY} ${CUDA_CUDART_LIBRARY} ${CUDA_NPP_LIBRARIES_ZED} )