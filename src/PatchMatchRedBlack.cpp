#include "PatchMatchRedBlack.h"


PatchMatchRedBlack::PatchMatchRedBlack(float alpha, float gamma, float tau_c, float tau_g) : PatchMatchBase(alpha, gamma, tau_c, tau_g)
{
}

void PatchMatchRedBlack::SpatialPropagation(int y, int x, int view, int iter)
{
	std::vector<std::pair<int, int>> offsets;
	offsets.push_back(std::make_pair(-1, 0));
	offsets.push_back(std::make_pair(0, -1));
	offsets.push_back(std::make_pair(1, 0));
	offsets.push_back(std::make_pair(0, 1));

	offsets.push_back(std::make_pair(-5, 0));
	offsets.push_back(std::make_pair(0, -5));
	offsets.push_back(std::make_pair(5, 0));
	offsets.push_back(std::make_pair(0, 5));

	/* offsets.push_back(std::make_pair(0 - y, 0)); */
	/* offsets.push_back(std::make_pair(0, 0 - x)); */
	/* offsets.push_back(std::make_pair(rows_ - 1 - y, 0)); */
	/* offsets.push_back(std::make_pair(0, cols_ - 1 - x)); */

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
		float new_cost = PlaneMatchCost(neigb_plane, y, x, view);
		/* printf("new cost %f, old_cost %f\n", new_cost, old_cost); */

		if(new_cost < old_cost)
		{
			old_plane = neigb_plane;
			old_cost = new_cost;
		}
	}
}


void PatchMatchRedBlack::StartIteration(int iter)
{
	printf("iter : %d\n", iter);
	for(int it = 0; it < iter; ++it)
	{
		// 奇数次迭代和偶数次迭代的，迭代方式是不同的
		bool iter_type = (it % 2 == 0);

		std::cerr << "Iteration " << (it + 1) << "/" << iter << "\r";
		for(int work_view = 0; work_view < 2; ++work_view)
		{
			for(int y = 0; y < rows_; ++y)
			{
				int x = (y % 2 == 0);
				for(; x < cols_; x += 2)
				{
					ProcessPixel(y, x, work_view, it);
				}
			}
			for(int y = 0; y < rows_; ++y)
			{
				int x = (y % 2 == 1);
				for(; x < cols_; x += 2)
				{
					ProcessPixel(y, x, work_view, it);
				}
			}
		}
	}
	std::cout << "\n";
}
