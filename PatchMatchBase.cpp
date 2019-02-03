#include "PatchMatchBase.h"
#include <random>

#include <time.h>


template<typename T>
inline T VecAverage(const T& x, const T& y, float wm)
{
	return wm * x + (1 - wm ) * y;
}




PatchMatchBase::PatchMatchBase(float alpha, float gamma, float tau_c, float  tau_g) 
	: alpha_(alpha), gamma_(gamma), tau_color_(tau_c), tau_gradient_(tau_g)
{
}

float PatchMatchBase::Weight(const cv::Vec3f& p, const cv::Vec3f& q, float gamma)
{
	float res = std::exp(-cv::norm(p - q, cv::NORM_L1) * 1.0f / gamma);
	return res;
}


void PatchMatchBase::PrecomputePixelsWeights(const cv::Mat& image, cv::Mat& weights, int win_size)
{
	int half = win_size / 2;
	/* #pragma omp parallel for */
	/* for(int y = 0; y < rows_; ++y) */
	/* 	for(int x = 0; x < cols_; ++x) */
	/* 		for(int i = -half; i <= half; ++i) */
	/* 			for(int j = -half; j <= half; ++j) */
	/* 			{ */
	/* 				int cx = x + j; */
	/* 				int cy = y + i; */
	/* 				if(cx >= 0 && cy >= 0 && cx < cols_ && cy < rows_) */
	/* 				{ */
	/* 					weights.at<float>(cv::Vec<int,4>{y, x, i + half, j + half}) = Weight(image.at<cv::Vec3b>(y, x), image.at<cv::Vec3b>(cy, cx), this->gamma_); */
	/* 				} */
	/* 			} */

		
	#pragma omp parallel for
	for(int cx=0; cx < image.cols; ++cx)
		for(int cy=0; cy < image.rows; ++cy)
			for(int x = cx - half; x <= cx + half; ++x)
				for(int y = cy - half; y <= cy + half; ++y)
					if(x >= 0 && y >= 0 && x < image.cols && y < image.rows)
					{
						weights.at<float>(cv::Vec<int,4> {cy, cx, y -cy +half, x -cx +half}) = Weight(image.at<cv::Vec3b>(cy, cx), image.at<cv::Vec3b>(y, x), this->gamma_);
					}

}


void PatchMatchBase::PrecomputeGrayscaleGradient(const cv::Mat& image, cv::Mat& grad_x, cv::Mat& grad_y)
{
	int scale = 1;
	int delta = 0;
	cv::Mat gray;
	cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
	cv::Sobel(gray, grad_x, CV_32F, 1, 0, 3, scale, delta, cv::BORDER_DEFAULT);
	cv::Sobel(gray, grad_y, CV_32F, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT);
	grad_x = grad_x / 8.0f;
	grad_y = grad_y / 8.0f;
}

void PatchMatchBase::InitializeRandomPlane(Matrix2D<DisparityPlane>& planes, float min_disparity, float max_disparity)
{
	cv::RNG random_generator;
	const int RAND_HALF = RAND_MAX / 2;
	for(int y = 0; y < rows_; ++y)
	{
		for(int x = 0; x < cols_; ++x)
		{
			float z = random_generator.uniform(min_disparity, max_disparity);
			cv::Vec3f point(x, y, z);

			float nx = ((float)std::rand() - RAND_HALF) / RAND_HALF;
			float ny = ((float)std::rand() - RAND_HALF) / RAND_HALF;
			float nz = ((float)std::rand() - RAND_HALF) / RAND_HALF;

			// other way to initialize
			/* float sum = 0.0f; */
			/* do */
			/* { */
			/* 	float x = random_generator.uniform(-1.0f, 1.0f); */
			/* 	float y = random_generator.uniform(-1.0f, 1.0f); */
			/* 	sum = std::sqrt(x * x + y * y); */
			/* }while(sum >= 1.0f); */

			/* const float sq = sqrt(1.0f - sum); */
			/* float nx = 2.0f * x * sq; */
			/* float ny = 2.0f * y * sq; */
			/* float nz = 1.0f - 2 * sum; */

			cv::Vec3f normal(nx, ny, nz);
			cv::normalize(normal, normal);

			planes(y, x) = DisparityPlane(point, normal);
		}
	}
}

void PatchMatchBase::EvaluatePlaneCost(int view)
{
	cv::Mat& costs = this->costs_[view];
	const Matrix2D<DisparityPlane>& planes = this->planes_[view];

	#pragma omp parallel for
	for(int y = 0; y < rows_; ++y)
	{
		for(int x = 0; x < cols_; ++x)
		{
			costs.at<float>(y, x) = PlaneMatchCost(planes(y, x), y, x, view);
		}
	}
}

float PatchMatchBase::PlaneMatchCost(const DisparityPlane& plane, int y, int x, int view)
{
	// left : -1;  right : 1;
	int sign = (view == 0) ? -1 : 1;

	float cost = 0;
	int half = win_size_ / 2;

	const cv::Mat& f1 = views_[view];
	const cv::Mat& f2 = views_[1 - view];
	const cv::Mat& gx1 = grad_xs_[view];
	const cv::Mat& gx2 = grad_xs_[1 - view];
	const cv::Mat& gy1 = grad_ys_[view];
	const cv::Mat& gy2 = grad_ys_[1 - view];
	const cv::Mat& w1 = weigs_[view];

	for(int i = -half; i <= half; ++i)
	{
		for(int j = -half; j <= half; ++j)
		{
			int sample_x = x + j;
			int sample_y = y + i;
			if(sample_x < 0 || sample_y < 0 || sample_x >= cols_ || sample_y >= rows_)
				continue;

			float d = plane.GetDisparity(sample_x, sample_y);
			if(d < min_disparity_ || d > max_disparity_)
			{
				cost += PLANE_PENALTY;
			}
			else
			{
				float corr_sample_x = sample_x + sign * d;
				int corr_sample_x_i = (int)corr_sample_x;

				float wm = 1 - (corr_sample_x - corr_sample_x_i);

				if(corr_sample_x_i > cols_ - 2)
					corr_sample_x_i = cols_ - 2;

				if(corr_sample_x_i < 0)
					corr_sample_x_i = 0;

				cv::Vec3f corr_color = VecAverage((cv::Vec3f)f2.at<cv::Vec3b>(sample_y, corr_sample_x_i), (cv::Vec3f)f2.at<cv::Vec3b>(sample_y, corr_sample_x_i + 1), wm);
				float corr_grad_x = VecAverage(gx2.at<float>(sample_y, corr_sample_x_i), gx2.at<float>(sample_y, corr_sample_x_i + 1), wm);
				float corr_grad_y = VecAverage(gy2.at<float>(sample_y, corr_sample_x_i), gy2.at<float>(sample_y, corr_sample_x_i + 1), wm);
				cv::Vec2f corr_grad(corr_grad_x, corr_grad_y);

				cv::Vec3f color = f1.at<cv::Vec3b>(sample_y, sample_x);
				float grad_x = gx1.at<float>(sample_y, sample_x);
				float grad_y = gy1.at<float>(sample_y, sample_x);
				cv::Vec2f grad(grad_x, grad_y);

				//ASW权重
				float w = w1.at<float>(cv::Vec<int, 4>{y, x, i + half, j + half});

				cost += w * Dissimilarity(color, corr_color, grad, corr_grad);
			}
		}
	}
	return cost;
}

float PatchMatchBase::Dissimilarity(const cv::Vec3f& color, const cv::Vec3f& corr_color, const cv::Vec2f& grad, const cv::Vec2f& corr_grad)
{
	float cost_color = cv::norm(color - corr_color, cv::NORM_L1);
	float cost_grad = cv::norm(grad - corr_grad, cv::NORM_L1);

	cost_color = std::min(cost_color, tau_color_);
	cost_grad = std::min(cost_grad, tau_gradient_);

	float cost = (1 - alpha_) * cost_color + alpha_ * cost_grad;
	return cost;
}

/* void PatchMatchBase::StartIteration(int iter) */
/* { */
/* 	printf("iter : %d\n", iter); */
/* 	for(int it = 0; it < iter; ++it) */
/* 	{ */
/* 		// 奇数次迭代和偶数次迭代的，迭代方式是不同的 */
/* 		bool iter_type = (iter % 2 == 0); */

/* 		std::cerr << "Iteration " << (it + 1) << "/" << iter << "\r"; */
/* 		for(int work_view = 0; work_view < 2; ++work_view) */
/* 		{ */
/* 			if(iter_type) */
/* 			{ */
/* 				for(int y = 0; y < rows_; ++y) */
/* 				{ */
/* 					for(int x = 0; x < cols_; ++x) */
/* 					{ */
/* 						ProcessPixel(y, x, work_view, iter); */
/* 					} */
/* 				} */
/* 			} */
/* 			else */
/* 			{ */
/* 				for(int y = rows_ - 1; y >= 0; --y) */
/* 				{ */
/* 					for(int x = cols_ - 1; x >= 0; --x) */
/* 					{ */
/* 						ProcessPixel(y, x, work_view, iter); */
/* 					} */
/* 				} */
/* 			} */
/* 		} */
/* 	} */
/* 	std::cout << "\n"; */
/* } */

void PatchMatchBase::ProcessPixel(int y, int x, int view, int iter)
{
	SpatialPropagation(y, x, view, iter);
	PlaneRefinement(y, x, view, max_disparity_ / 2.0f, 1.0f, 0.1f);
	ViewPropagation(y, x, view);
}

/* void PatchMatchBase::SpatialPropagation(int y, int x, int view, int iter) */
/* { */
/* 	std::vector<std::pair<int, int>> offsets; */
/* 	if(iter % 2 == 0) */
/* 	{ */
/* 		offsets.push_back(std::make_pair(-1, 0)); */
/* 		offsets.push_back(std::make_pair(0, -1)); */
/* 	} */
/* 	else */
/* 	{ */
/* 		offsets.push_back(std::make_pair(1, 0)); */
/* 		offsets.push_back(std::make_pair(0, 1)); */
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
void PatchMatchBase::PlaneRefinement(int y, int x, int view, float max_delta_z, float max_delta_n, float end_z)
{
	int sign = (view== 0) ? -1 : 1;	// -1 processing left, +1 processing right
	DisparityPlane& old_plane = planes_[view](y, x);
	float& old_cost = costs_[view].at<float>(y, x);

	while(max_delta_z >= end_z)
	{
		std::random_device rd;
		std::mt19937 gen(rd());

		std::uniform_real_distribution<> rand_z(-max_delta_z, max_delta_z);
		std::uniform_real_distribution<> rand_n(-max_delta_n, max_delta_n);

		float z = old_plane.GetDisparity(x, y);
		float delta_z = rand_z(gen);
		cv::Vec3f new_point(x, y, z + delta_z);

		cv::Vec3f normal = old_plane.GetNormal();
		cv::Vec3f delta_n(rand_n(gen), rand_n(gen), rand_n(gen));
		cv::Vec3f new_normal = normal + delta_n;
		cv::normalize(new_normal, new_normal);

		DisparityPlane new_plane(new_point, new_normal);
		float new_cost = PlaneMatchCost(new_plane, y, x, view);

		if(new_cost < old_cost)
		{
			old_plane = new_plane;
			old_cost = new_cost;
		}
		max_delta_z /= 2.0f;
		max_delta_n /= 2.0f;
	}
}


void PatchMatchBase::PlaneRefinement2(int y, int x, int view, float max_delta_z, float max_delta_n, float end_z)
{
	int sign = (view== 0) ? -1 : 1;	// -1 processing left, +1 processing right
	DisparityPlane& old_plane = planes_[view](y, x);
	float& old_cost = costs_[view].at<float>(y, x);

	while(max_delta_z >= end_z)
	{
		std::random_device rd;
		std::mt19937 gen(rd());

		std::uniform_real_distribution<> rand_z(-max_delta_z, max_delta_z);
		std::uniform_real_distribution<> rand_n(-max_delta_n, max_delta_n);

		float z = old_plane.GetDisparity(x, y);
		float delta_z = rand_z(gen);
		cv::Vec3f point(x, y, z + delta_z);
		cv::Vec3f normal = old_plane.GetNormal();

		DisparityPlane new_plane(point, normal);
		float new_cost = PlaneMatchCost(new_plane, y, x, view);
		if(new_cost < old_cost)
		{
			old_plane = new_plane;
			old_cost = new_cost;
		}

		float delta_nx = rand_n(gen);
		float delta_ny = rand_n(gen);
		float delta_nz = rand_n(gen);

		point = old_plane.GetPoint();
		cv::Vec3f new_normal = normal;
		new_normal(0) += delta_nx;
		new_plane = DisparityPlane(point, new_normal);
		new_cost = PlaneMatchCost(new_plane, y, x, view);
		if(new_cost < old_cost)
		{
			old_plane = new_plane;
			old_cost = new_cost;
		}

		new_normal = old_plane.GetNormal();
		new_normal(1) += delta_ny;
		new_plane = DisparityPlane(point, new_normal);
		new_cost = PlaneMatchCost(new_plane, y, x, view);
		if(new_cost < old_cost)
		{
			old_plane = new_plane;
			old_cost = new_cost;
		}

		new_normal = old_plane.GetNormal();
		new_normal(2) += delta_ny;
		new_plane = DisparityPlane(point, new_normal);
		new_cost = PlaneMatchCost(new_plane, y, x, view);
		if(new_cost < old_cost)
		{
			old_plane = new_plane;
			old_cost = new_cost;
		}

		max_delta_z /= 2.0f;
		max_delta_n /= 2.0f;
	}
}

void PatchMatchBase::ViewPropagation(int y, int x, int view)
{
	int sign = (view == 0) ? -1 : 1;
	DisparityPlane view_plane = planes_[view](y, x);

	int mx, my;
	DisparityPlane new_plane = view_plane.ViewTransform(x, y, sign, mx, my);

	if(mx < 0 || my < 0 || mx >= cols_ || my >= rows_)
		return;

	float& old_cost = costs_[1 - view].at<float>(my, mx);
	float  new_cost = PlaneMatchCost(new_plane, my, mx, 1 - view);
	if(new_cost < old_cost)
	{
		planes_[1 - view](my, mx) = new_plane;
		old_cost = new_cost;
	}
}


void PatchMatchBase::CheckValidity(int view)
{
	cv::Mat& validity = validity_[view];
	validity = cv::Mat::zeros(rows_, cols_, CV_8U);
	int sign = (view == 0) ? -1 : 1;
	for(int y = 0; y < rows_; ++y)
	{
		for(int x = 0; x < cols_; ++x)
		{
			float disp = disps_[view].at<float>(y, x);
			float corr_x = x + sign * disp;
			int corr_x_i = (int)corr_x;
			float wm = 1 - (corr_x - corr_x_i);

			if(corr_x_i > cols_ - 2)
				corr_x_i = cols_ - 2;
			if(corr_x_i < 0)
				corr_x_i = 0;

			float corr_disp = VecAverage(disps_[1 - view].at<float>(y, corr_x_i), disps_[1 - view].at<float>(y, corr_x_i + 1), wm);
			
			validity.at<uchar>(y, x) = (std::fabs(disp - corr_disp) <= 1);
		}
	}
}


bool PatchMatchBase::FillInvalidPixel(int y, int x, int view)
{
	cv::Mat& validity = validity_[view];

	int x_left = x - 1;
	int x_right = x + 1;
	int y_up = y - 1;
	int y_down = y + 1;

	while(x_left >= 0 && !validity.at<uchar>(y, x_left))
		--x_left;
	while(x_right < cols_ && !validity.at<uchar>(y, x_right))
		++x_right;
	while(y_up >= 0 && !validity.at<uchar>(y_up, x))
			--y_up;
	while(y_down < rows_  && !validity.at<uchar>(y_down, x))
			++y_down;

		
	float disp_left = (x_left >= 0) ? planes_[view](y, x_left).GetDisparity(x, y) : (1 << 30);
	float disp_right = (x_right < cols_) ? planes_[view](y, x_right).GetDisparity(x, y) : (1 << 30);
	float disp_up = (y_up >= 0) ? planes_[view](y_up, x).GetDisparity(x, y) : (1 << 30);
	float disp_down = (y_down < rows_) ? planes_[view](y_down, x).GetDisparity(x, y) : (1 << 30);

	disps_[view].at<float>(y, x) = std::min(std::min(disp_left, disp_right), std::min(disp_up, disp_down));
	return true;


	int best_plane_x = x;
	if(x_left >= 0 && x_right < cols_)
	{
		float disp_left = planes_[view](y, x_left).GetDisparity(x, y);
		float disp_right = planes_[view](y, x_right).GetDisparity(x, y);
		/* best_plane_x = (disp_left < disp_right) ? x_left : x_right; */
		disps_[view].at<float>(y, x) = std::min(disp_left, disp_right);
	}
	else if(x_left >= 0)
	{
		float disp_left = planes_[view](y, x_left).GetDisparity(x, y);
		disps_[view].at<float>(y, x) = disp_left;
		/* best_plane_x = x_left; */
	}
	else if(x_right < cols_)
	{
		float disp_right = planes_[view](y, x_right).GetDisparity(x, y);
		disps_[view].at<float>(y, x) = disp_right;
		/* best_plane_x = x_right; */
	}
	else
	{
		return false;
	}
	return true;
	/* validity.at<uchar>(y, x) = (best_plane_x != x); */
	/* planes_[view](y, x) = planes_[view](y, best_plane_x); */
	/* disps_[view].at<float>(y, x) = planes_[view](y, x).GetDisparity(x, y); */

	return best_plane_x != x;
}
int PatchMatchBase::FillInvalidPixels(int view)
{
	cv::Mat& validity = validity_[view];

	int num_filled = 0;
	for(int y = 0; y < rows_; ++y)
	{
		for(int x = 0; x < cols_; ++x)
		{
			if(validity.at<uchar>(y, x))
				continue;
			num_filled += FillInvalidPixel(y, x, view);
		}
	}
	return num_filled;
}


void PatchMatchBase::WeightedMeidanFilter(int y, int x, int view)
{
	int half = win_size_ / 2;
	float total_w = 0;
	std::vector<std::pair<float, float>> disps_w;

	for(int i = -half; i <= half; ++i)
	{
		for(int j = -half; j <= half; ++j)
		{
			int sample_x = x + j;
			int sample_y = y + i;
			
			if(sample_x < 0 || sample_y < 0 || sample_x >= cols_ || sample_y >= rows_)
				continue;
			if(!validity_[view].at<uchar>(sample_y, sample_x))
				continue;

			cv::Vec<int, 4> w_ids({y, x, i + half, j + half});
			float w = weigs_[view].at<float>(w_ids);
			total_w += w;
			disps_w.push_back(std::make_pair(w, disps_[view].at<float>(sample_y, sample_x)));
		}
	}

	std::sort(disps_w.begin(), disps_w.end());
	float w = 0;
	float median_w = total_w / 2.0;
	
	// 普通的中值滤波只是得到中间的点
	// 但是这里因为使用的ASW， 所以，这里的中值点，要根据权重来得到
	for(auto dw=disps_w.begin(); dw < disps_w.end(); ++dw)
	{
		w += dw->first;
		
		if(w >= median_w)
		{
			if(dw == disps_w.begin())
			{
				disps_[view].at<float>(y, x) = dw->second;
			}	
			else
			{
				// 每个点的视差，等于该窗口中的的中值点视差
				disps_[view].at<float>(y, x) = ((dw - 1)->second + dw->second) / 2.0f;
				/* disparity(cy, cx) = ((dw-1)->second + dw->second) / 2.0f; */
			}
		}
	}

}

void PatchMatchBase::WeightedMeidanFilters(int view)
{
	for(int y = 0; y < rows_; ++y)
	{
		for(int x = 0; x < cols_; ++x)
		{
			WeightedMeidanFilter(y, x, view);
		}
	}
}
void PatchMatchBase::PostProcess()
{
	CheckValidity(0);
	CheckValidity(1);

	int num_filled_left = FillInvalidPixels(0);
	int num_filled_right = FillInvalidPixels(1);

	WeightedMeidanFilters(0);
	WeightedMeidanFilters(1);
	/* printf("%d %d\n", num_filled_left, num_filled_right); */

	/* validity_[0] *= 255; */
	/* validity_[1] *= 255; */
	/* cv::imshow("left_validity", validity_[0]); */
	/* cv::imshow("right_validity", validity_[1]); */
	/* cv::waitKey(0); */

}
void PatchMatchBase::PlaneToDisparity(int view)
{
	for(int y = 0; y < rows_; ++y)
	{
		for(int x = 0; x < cols_; ++x)
		{
			disps_[view].at<float>(y, x) = planes_[view](y, x).GetDisparity(x, y);
		}
	}
}
void PatchMatchBase::Process(const cv::Mat& image1, const cv::Mat& image2, int win_size, int min_disparity, int max_disparity, int iter)
{
	this->views_[0] = image1;
	this->views_[1] = image2;

	this->rows_ = image1.rows;
	this->cols_ = image1.cols;

	this->win_size_ = win_size;
	this->min_disparity_ = min_disparity;
	this->max_disparity_ = max_disparity;

	clock_t start, end;

	std::cerr << "Precomputing pixels weights...\n";
	start = clock();
	int win_mat_size[] = {this->rows_, this->cols_, this->win_size_, this->win_size_};
	this->weigs_[0] = cv::Mat(4, win_mat_size, CV_32F);
	this->weigs_[1] = cv::Mat(4, win_mat_size, CV_32F);
	this->PrecomputePixelsWeights(image1, this->weigs_[0], this->win_size_);
	this->PrecomputePixelsWeights(image2, this->weigs_[1], this->win_size_);
	end = clock();
	/* std::cout << "time : " << (end - start) / CLOCKS_PER_SEC << std::endl; */
	

	std::cerr << "Computeing images gradient...\n";
	start = clock();
	this->PrecomputeGrayscaleGradient(image1, this->grad_xs_[0], this->grad_ys_[0]);
	this->PrecomputeGrayscaleGradient(image2, this->grad_xs_[1], this->grad_ys_[1]);
	end = clock();
	/* std::cout << "time : " << (end - start) / CLOCKS_PER_SEC << std::endl; */

	/* cv::imshow("grad_x1", this->grad_xs_[0]); */
	/* cv::imshow("grad_x2", this->grad_xs_[1]); */
	/* cv::imshow("grad_y1", this->grad_ys_[0]); */
	/* cv::imshow("grad_y2", this->grad_ys_[1]); */
	/* cv::waitKey(0); */

	std::cerr << "Precomputing random planes...\n";
	start = clock();
	this->planes_[0] = Matrix2D<DisparityPlane>(this->rows_, this->cols_);
	this->planes_[1] = Matrix2D<DisparityPlane>(this->rows_, this->cols_);
	this->InitializeRandomPlane(this->planes_[0], this->min_disparity_, this->max_disparity_);
	this->InitializeRandomPlane(this->planes_[1], this->min_disparity_, this->max_disparity_);
	end = clock();
	/* std::cout << "time : " << (end - start) / CLOCKS_PER_SEC << std::endl; */

	std::cerr << "Evaluating initial planes cost...\n";
	start = clock();
	this->costs_[0] = cv::Mat(this->rows_, this->cols_, CV_32F);
	this->costs_[1] = cv::Mat(this->rows_, this->cols_, CV_32F);
	this->EvaluatePlaneCost(0);
	this->EvaluatePlaneCost(1);
	end = clock();
	/* std::cout << "time : " << (end - start) / CLOCKS_PER_SEC << std::endl; */


	
	this->current_iter_index_ = 0;
	std::cerr << "Processing left and right view...\n";
	start = clock();
	StartIteration(iter);
	end = clock();
	/* std::cout << "time : " << (end - start) / CLOCKS_PER_SEC << std::endl; */

	std::cerr << "Plane to disparitys...\n";
	start = clock();
	this->disps_[0] = cv::Mat(this->rows_, this->cols_, CV_32F);
	this->disps_[1] = cv::Mat(this->rows_, this->cols_, CV_32F);
	PlaneToDisparity(0);
	PlaneToDisparity(1);
	end = clock();
	/* std::cout << "time : " << (end - start) / CLOCKS_PER_SEC << std::endl; */
}


cv::Mat PatchMatchBase::GetLeftDisparityMap()
{
	return disps_[0];
}
cv::Mat PatchMatchBase::GetRightDisparityMap()
{
	return disps_[1];
}
