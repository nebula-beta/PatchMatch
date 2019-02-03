cmake_minimum_required(VERSION 2.8)

project(PatchMatch)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")



find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_library(
	patch_match	
	# Matrix2D.h Matrix2D.cpp
	Matrix2D.h
	DisparityPlane.h DisparityPlane.cpp
	PatchMatchBase.h PatchMatchBase.cpp
	PatchMatch.h PatchMatch.cpp
	PatchMatchRowColSweep.h PatchMatchRowColSweep.cpp
	PatchMatchRedBlack.h PatchMatchRedBlack.cpp
)


add_executable(pm pm.cpp)
target_link_libraries(pm ${OpenCV_LIBS} patch_match)

add_executable(pm_rows_cols_sweep pm_rows_cols_sweep.cpp)
target_link_libraries(pm_rows_cols_sweep ${OpenCV_LIBS} patch_match)

add_executable(pm_red_black pm_red_black.cpp)
target_link_libraries(pm_red_black ${OpenCV_LIBS} patch_match)
