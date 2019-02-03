#ifndef __DISPARITY_PLANE_H__
#define __DISPARITY_PLANE_H__

#include <opencv2/opencv.hpp>
class DisparityPlane
{
public:
	
	DisparityPlane();
	// 平面是由一个3D点和法向量所决定
	DisparityPlane(cv::Vec3f point, cv::Vec3f normal);
	
	/* float operator[](int idx) const; */
	/* cv::Vec3f operator()(); */
	
	cv::Vec3f GetPoint() const;
	cv::Vec3f GetNormal() const;
	cv::Vec3f GetCoeff() const;
	float GetDisparity(float x, float y) const;
	
	DisparityPlane ViewTransform(int x, int y, int sign, int& qx, int &qy);
	
private:
	
	cv::Vec3f point;
	cv::Vec3f normal;
	cv::Vec3f coeff;
};

#endif //__DISPARITY_PLANE_H__
