cmake_minimum_required(VERSION 2.8)

project(PatchMatch)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")

include_directories(${PROJECT_SOURCE_DIR}/include)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build)



find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})



add_subdirectory(src)

