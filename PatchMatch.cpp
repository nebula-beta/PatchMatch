#include "PatchMatch.h"


PatchMatch::PatchMatch(float alpha, float gamma, float tau_c, float tau_g) : PatchMatchBase(alpha, gamma, tau_c, tau_g)
{
}
/* void PatchMatch::SpatialPropagation(int y, int x, int view, int iter) */
/* { */
/* 	std::vector<std::pair<int, int>> offsets; */
/* 	if(iter % 2 == 0) */
/* 	{ */
/* 		offsets.push_back(std::make_pair(-1, 0)); */
/* 		offsets.push_back(std::make_pair(0, -1)); */
/* 		offsets.push_back(std::make_pair(-5, 0)); */
/* 		offsets.push_back(std::make_pair(0, -5)); */
/* 	} */
/* 	else */
/* 	{ */
/* 		offsets.push_back(std::make_pair(1, 0)); */
/* 		offsets.push_back(std::make_pair(0, 1)); */
/* 		offsets.push_back(std::make_pair(5, 0)); */
/* 		offsets.push_back(std::make_pair(0, 5)); */
/* 	} */
/* 	DisparityPlane& old_plane = planes_[view](y, x); */
/* 	float& old_cost = costs_[view].at<float>(y, x); */

/* 	for(auto it = offsets.begin(); it < offsets.end(); ++it) */
/* 	{ */
/* 		std::pair<int, int> ofs = *it; */
/* 		int ny = y + ofs.first; */
/* 		int nx = x + ofs.second; */
/* 		if(nx < 0 || ny < 0 || nx >= cols_ || ny >= rows_) */
/* 			continue; */

/* 		DisparityPlane& neigb_plane = planes_[view](ny, nx); */
/* 		float new_cost = PlaneMatchCost(neigb_plane, y, x, view); */
/* 		/1* printf("new cost %f, old_cost %f\n", new_cost, old_cost); *1/ */

/* 		if(new_cost < old_cost) */
/* 		{ */
/* 			old_plane = neigb_plane; */
/* 			old_cost = new_cost; */
/* 		} */
/* 	} */
/* } */
void PatchMatch::SpatialPropagation(int y, int x, int view, int iter)
{
	std::vector<std::pair<int, int>> offsets;
	if(iter % 2 == 0)
	{
		offsets.push_back(std::make_pair(-1, 0));
		offsets.push_back(std::make_pair(0, -1));
		offsets.push_back(std::make_pair(-5, 0));
		offsets.push_back(std::make_pair(0, -5));
	}
	else
	{
		offsets.push_back(std::make_pair(1, 0));
		offsets.push_back(std::make_pair(0, 1));
		offsets.push_back(std::make_pair(5, 0));
		offsets.push_back(std::make_pair(0, 5));
	}
	DisparityPlane& old_plane = planes_[view](y, x);
	float& old_cost = costs_[view].at<float>(y, x);

	for(auto it = offsets.begin(); it < offsets.end(); ++it)
	{
		std::pair<int, int> ofs = *it;
		int ny = y + ofs.first;
		int nx = x + ofs.second;
		if(nx < 0 || ny < 0 || nx >= cols_ || ny >= rows_)
			continue;

		DisparityPlane& neigb_plane = planes_[view](ny, nx);
		cv::Vec3f neigb_point = neigb_plane.GetPoint();

		// 把邻居像素的视差和法向量传递给自己
		DisparityPlane new_plane(cv::Vec3f(x, y, neigb_point(2)), neigb_plane.GetNormal());
		
		float new_cost = PlaneMatchCost(new_plane, y, x, view);
		if(new_cost < old_cost)
		{
			old_plane = new_plane;
			old_cost = new_cost;
		}
		
	}
}


void PatchMatch::StartIteration(int iter)
{
	printf("iter : %d\n", iter);
	for(int it = 0; it < iter; ++it)
	{
		// 奇数次迭代和偶数次迭代的，迭代方式是不同的
		bool iter_type = (it % 2 == 0);

		std::cerr << "Iteration " << (it + 1) << "/" << iter << "\r";
		for(int work_view = 0; work_view < 2; ++work_view)
		{
			if(iter_type)
			{
				for(int y = 0; y < rows_; ++y)
				{
					for(int x = 0; x < cols_; ++x)
					{
						ProcessPixel(y, x, work_view, it);
					}
				}
			}
			else
			{
				for(int y = rows_ - 1; y >= 0; --y)
				{
					for(int x = cols_ - 1; x >= 0; --x)
					{
						ProcessPixel(y, x, work_view, it);
					}
				}
			}
		}
	}
	std::cout << "\n";
}

