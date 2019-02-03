



add_library(
	patch_match	
	# Matrix2D.h Matrix2D.cpp
	../include/Matrix2D.h
	../include/DisparityPlane.h DisparityPlane.cpp
	../include/PatchMatchBase.h PatchMatchBase.cpp
	../include/PatchMatch.h PatchMatch.cpp
	../include/PatchMatchRowColSweep.h PatchMatchRowColSweep.cpp
	../include/PatchMatchRedBlack.h PatchMatchRedBlack.cpp
)

add_executable(pm pm.cpp)
target_link_libraries(pm ${OpenCV_LIBS} patch_match)

add_executable(pm_rows_cols_sweep pm_rows_cols_sweep.cpp)
target_link_libraries(pm_rows_cols_sweep ${OpenCV_LIBS} patch_match)

add_executable(pm_red_black pm_red_black.cpp)
target_link_libraries(pm_red_black ${OpenCV_LIBS} patch_match)
