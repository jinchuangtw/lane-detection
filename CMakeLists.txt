cmake_minimum_required(VERSION 2.8...3.20)
project( LaneDetection )
SET(CMAKE_BUILD_TYPE "Release")

find_package( OpenCV 3 REQUIRED )
add_executable( LaneDetection main.cpp )
target_link_libraries( LaneDetection ${OpenCV_LIBS} )