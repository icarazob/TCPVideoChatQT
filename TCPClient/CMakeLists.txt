SET(execName TCPClient)
CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

project(${execName})

SET (SOURCES
	main.cpp
	TCPClient.cpp
)

SET (HEADERS
	TCPClient.h
)

add_executable(${execName} ${SOURCES} ${HEADERS})
add_definitions(-std=c++11 -g -O3)