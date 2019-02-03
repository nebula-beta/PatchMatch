#ifndef __PATCH_MATCH_BASE_H__
#define __PATCH_MATCH_BASE_H__
#include "Matrix2D.h"
#include "DisparityPlane.h"
#include <opencv2/opencv.hpp>


const int PLANE_PENALTY = 120;

// step1 : initialization
// step2 ：spatial propagation
// step3 ：view propagation
// step4 ：post-processing
// step5 : generate dispaR
//

class PatchMatchBase
{
public:
	PatchMatchBase(float alpha, float gamma, float tau_c, float tau_g);
	float Weight(const cv::Vec3f& p, const cv::Vec3f& q, float gamma = 10.0f);
	void PrecomputePixelsWeights(const cv::Mat& image, cv::Mat& weights, int win_size);
	void PrecomputeGrayscaleGradient(const cv::Mat& image, cv::Mat& grad_x, cv::Mat& grad_y);
	void InitializeRandomPlane(Matrix2D<DisparityPlane>& planes, float min_disparity, float max_disparity);
	void EvaluatePlaneCost(int view);
	float PlaneMatchCost(const DisparityPlane& plane, int y, int x, int view);
	float Dissimilarity(const cv::Vec3f& color, const cv::Vec3f& corr_color, const cv::Vec2f& grad, const cv::Vec2f& corr_grad);
	virtual void StartIteration(int iter_num) = 0;
	void ProcessPixel(int y, int x, int view, int iter);
	virtual void SpatialPropagation(int y, int x, int view, int iter) = 0;
	void PlaneRefinement(int y, int x, int view, float max_delta_z, float max_delta_n, float end_z);
	void PlaneRefinement2(int y, int x, int view, float max_delta_z, float max_delta_n, float end_z);
	void ViewPropagation(int y, int x, int view);

	void CheckValidity( int view);
	bool FillInvalidPixel(int y, int x, int view);
	int FillInvalidPixels(int view);
	void WeightedMeidanFilter(int y, int x, int view);
	void WeightedMeidanFilters(int view);
	void PostProcess();
	void PlaneToDisparity(int view);

	void Process(const cv::Mat& image1, const cv::Mat& image2, int win_size, int min_disparity, int max_disparity, int iter);



	cv::Mat GetLeftDisparityMap();
	cv::Mat GetRightDisparityMap();
	
protected:
	float alpha_;
	float gamma_;
	float tau_color_;
	float tau_gradient_;

	int win_size_;
	int min_disparity_;
	int max_disparity_;

	cv::Mat views_[2];
	cv::Mat grad_xs_[2];
	cv::Mat grad_ys_[2];
	cv::Mat grads_[2];
	cv::Mat weigs_[2];
	Matrix2D<DisparityPlane> planes_[2];
	cv::Mat costs_[2];
	cv::Mat disps_[2];
	cv::Mat validity_[2];

	int rows_;
	int cols_;

	int current_iter_index_;
};





#endif  //__PATCH_MATCH_H__
