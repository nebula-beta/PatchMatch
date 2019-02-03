#include "PatchMatchRowColSweep.h"

PatchMatchRowColSweep::PatchMatchRowColSweep(float alpha, float gamma, float tau_c, float tau_g)
	: PatchMatchBase(alpha, gamma, tau_c, tau_g)
{
}


void PatchMatchRowColSweep::SpatialPropagation(int y, int x, int view, int iter)
{
	int iter_type = (iter % 4);
	std::vector<std::pair<int, int>> offsets;
	// rows
	if(iter_type == 0)
	{
		offsets.push_back(std::make_pair(0, -1));
		offsets.push_back(std::make_pair(0, -5));
	}
	// cols
	else if(iter_type == 1)
	{
		offsets.push_back(std::make_pair(-1, 0));
		offsets.push_back(std::make_pair(-5, 0));
	}
	// reverse rows
	else if(iter_type == 2)
	{
		offsets.push_back(std::make_pair(0, 1));
		offsets.push_back(std::make_pair(0, 5));
	}
	// reverse cols
	else if(iter_type == 3)
	{
		offsets.push_back(std::make_pair(1, 0));
		offsets.push_back(std::make_pair(5, 0));
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
		float new_cost = PlaneMatchCost(neigb_plane, y, x, view);
		/* printf("new cost %f, old_cost %f\n", new_cost, old_cost); */

		if(new_cost < old_cost)
		{
			old_plane = neigb_plane;
			old_cost = new_cost;
		}
	}
}


void PatchMatchRowColSweep::StartIteration(int iter)
{
	printf("iter : %d\n", iter);
	for(int it = 0; it < iter; ++it)
	{
		// 奇数次迭代和偶数次迭代的，迭代方式是不同的
		int iter_type = (it % 4);

		// row -> col -> reverse row -> reverse col
		std::cerr << "Iteration " << (it + 1) << "/" << iter << "\r";
		for(int work_view = 0; work_view < 2; ++work_view)
		{
			// rows
			if(iter_type == 0)
			{
				for(int y = 0; y < rows_; ++y)
				{
					for(int x = 0; x < cols_; ++x)
					{
						ProcessPixel(y, x, work_view, it);
					}
				}
			}
			// cols
			else if(iter_type == 1)
			{
				for(int x = 0; x < cols_; ++x)
				{
					for(int y = 0; y < rows_; ++y)
					{
						ProcessPixel(y, x, work_view, it);
					}
				}
			}
			// reverse rows
			else if(iter_type == 2)
			{
				for(int y = rows_ - 1; y >= 0; --y)
				{
					for(int x = cols_ - 1; x >= 0; --x)
					{
						ProcessPixel(y, x, work_view, it);
					}
				}
			}
			// reverse cols
			else if(iter_type == 3)
			{
				for(int x = cols_ - 1; x >= 0; --x)
				{
					for(int y = rows_ - 1; y >= 0; --y)
					{
						ProcessPixel(y, x, work_view, it);
					}
				}
			}
		}
	}
	std::cout << "\n";
}

