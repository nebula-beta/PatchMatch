#include "DisparityPlane.h"


DisparityPlane::DisparityPlane() {}


DisparityPlane::DisparityPlane(cv::Vec3f point, cv::Vec3f normal) : point(point), normal(normal)
{
	float a = -normal[0] / normal[2];
	float b = -normal[1] / normal[2];
	float c = cv::sum(normal.mul(point))[0] / normal[2];
	coeff = cv::Vec3f(a, b, c);
}

/* inline float DisparityPlane::operator[](int idx) const { return coeff[idx]; } */

/* inline cv::Vec3f DisparityPlane::operator()() { return coeff; } */

cv::Vec3f DisparityPlane::GetPoint() const { return point; }

cv::Vec3f DisparityPlane::GetNormal() const { return normal; }

cv::Vec3f DisparityPlane::GetCoeff() const { return coeff; }

float DisparityPlane::GetDisparity(float x, float y) const
{
	/* float disp = coeff[0] * x + coeff[1] * y + coeff[2]; */
	float disp = coeff(0) * x + coeff(1) * y + coeff(2);
	return disp;
}



// 将一个平面变成另外一个平面， 变成哪个平面呢???
DisparityPlane DisparityPlane::ViewTransform(int x, int y, int sign, int& qx, int &qy)
{
	// 计算得到视差
	float z = coeff[0] * x + coeff[1] * y + coeff[2];
	// 根据视差得到在另个平面上的对应点
	qx = x + sign * z;
	qy = y;
	
	// 另一个平面上的对应点所对应的视差平面
	cv::Vec3f p(qx, qy, z);
	return DisparityPlane(p, this->normal);
}
