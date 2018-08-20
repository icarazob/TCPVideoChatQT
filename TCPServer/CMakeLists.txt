SET(execName TCPServer)
CMAKE_MINIMUM_REQUIRED(VERSION 2.4)
PROJECT(${execName})


SET (SOURCES 
	TCPServer.cpp
	main.cpp
)

SET (HEADERS
	TCPServer.h
)

#include_directories(${CUDA_INCLUDE_DIRS})
#include_directories(${ZED_INCLUDE_DIRS})
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

#link_directories(${ZED_LIBRARY_DIR})
#link_directories(${CUDA_LIBRARY_DIRS})

ADD_EXECUTABLE(${execName} ${SOURCES} ${HEADERS})
add_definitions(-std=c++11 -g -O3)

#TARGET_LINK_LIBRARIES(${execName}
 #                       ${ZED_LIBRARIES}
  #                      ${CUDA_CUDA_LIBRARY} ${CUDA_CUDART_LIBRARY} ${CUDA_NPP_LIBRARIES_ZED} )