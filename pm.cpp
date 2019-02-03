#include "DisparityPlane.h"
#include "Matrix2D.h"
#include "PatchMatch.h"

#include <algorithm>
#include <opencv2/opencv.hpp>

#include <vector>
#include <iostream>
#include <vector>
#include <string>
#include <time.h>

#include <sys/stat.h>
#include <sys/types.h>
float Evaluate(cv::Mat standard, cv::Mat myMap) 
{
	double sum = standard.rows * standard.cols;
	int count = 0;
	int ans = 0;
	for (int i = 0; i < standard.rows; i ++) 
	{
		for (int j = 0; j < standard.cols; j ++) 
		{
			ans = standard.ptr<uchar>(i)[j] - myMap.ptr<float>(i)[j];
			//与原图灰度相差大于1可认为是bad pixels，因为增强对比度，所以disparity maps都乘以3显示
			if (ans > 3 || ans < -3) count ++;
		}
	}
	double result = (count + 0.0)/sum;
	std::cout << std::setiosflags(std::ios::fixed);
	std::cout << std::setprecision(2) << result * 100 << "\\%" << std::endl;
	return result * 100;
}

bool check_image(const cv::Mat &image, std::string name="Image")
{
	if(!image.data)
	{
		std::cerr <<name <<" data not loaded.\n";
		return false;
	}
	return true;
}


bool check_dimensions(const cv::Mat &img1, const cv::Mat &img2)
{
	if(img1.cols != img2.cols or img1.rows != img2.rows)
	{
		std::cerr << "Images' dimensions do not corresponds.";
		return false;
	}
	return true;
}



std::vector<std::string> name = {"Aloe", "Baby1", "Baby2", "Baby3", "Bowling1",
"Bowling2", "Cloth1", "Cloth2", "Cloth3", "Cloth4", "Flowerpots",
"Lampshade1", "Lampshade2", "Midd1", "Midd2", "Monopoly",
"Plastic", "Rocks1", "Rocks2", "Wood1", "Wood2"};


int main(int argc, char** argv)
{
	//  参数
	const float alpha =  0.9f;
	const float gamma = 10.0f;
	const float tau_c = 10.0f;
	const float tau_g =  2.0f;
	

	mkdir("resultImages1", S_IRWXU);

	const time_t start = time(NULL);

	for (int i = 0; i < name.size(); i ++)
	{

		if(i > 0)
			break;
		std::cout << name[i] << std::endl;
	//在目的文件夹中创建相应的文件夹，以便存入图片
		std::string str = "resultImages1/" + name[i];
		const char * dir = str.c_str();
		mkdir(dir, S_IRWXU);

		//读取images文件夹中的源图片
		cv::Mat img1 = cv::imread("dataset/" + name[i] + "/view1.png" , cv::IMREAD_COLOR );
		cv::Mat img2 = cv::imread("dataset/" + name[i] + "/view5.png" , cv::IMREAD_COLOR );

		if ( (!img1.data) || (!img2.data))

		{
			printf("Please input right data~~\n");
			return -1;
		}
		
		// Image loading check
		if(!check_image(img1, "Image 1") or !check_image(img2, "Image 2"))
			return 1;
		
		// Image sizes check
		if(!check_dimensions(img1, img2))
			return 1;
		
		// processing images
		// 初始化参数
		PatchMatch patch_match(alpha, gamma, tau_c, tau_g);
		
		//11, 21, 31
		int win_size = 31;
		int min_disparity = 1;
		int max_disparity = 80;
		std::vector<float> error_rate1;
		std::vector<float> error_rate2;
		for(int iter = 1; iter <= 10; ++iter)
		{
		/* int iter = 1; */
			clock_t start, end;
			start = clock();
			patch_match.Process(img1, img2, win_size, min_disparity, max_disparity, iter);
			patch_match.PostProcess();
			end = clock();

			std::cout << "time : " << (end - start) / CLOCKS_PER_SEC << std::endl;
			// 得到左右图片的视差图
			cv::Mat disp1 = patch_match.GetLeftDisparityMap();
			cv::Mat disp2 = patch_match.GetRightDisparityMap();

			
			
			try
			{
				disp1 = disp1 * 3;
				disp2 = disp2 * 3;

				cv::imwrite( "resultImages1/" + name[i] + "/" + name[i] + "_disp1.png", disp1);
				cv::imwrite( "resultImages1/" + name[i] + "/" + name[i] + "_disp5.png", disp2);

				cv::Mat standardLeft = cv::imread("dataset/" + name[i] + "/disp1.png", -1);
				cv::Mat standardRight = cv::imread("dataset/" + name[i] + "/disp5.png", -1);

				float error_rate_left = Evaluate(standardLeft, disp1);
				float error_rate_right = Evaluate(standardRight, disp2);
				error_rate1.push_back(error_rate_left);
				error_rate2.push_back(error_rate_right);

			} 
			catch(std::exception &e)
			{
				std::cerr << "Disparity save error.\n" <<e.what();
				return 1;
			}
		}
		for(int i = 0; i <  error_rate1.size(); ++i)
		{
			if(i == 0)
				printf("[%f,", error_rate1[i]);
			else if(i == error_rate1.size() - 1)
				printf("%f]\n", error_rate1[i]);
			else
				printf("%f, ", error_rate1[i]);
		}
		for(int i = 0; i <  error_rate2.size(); ++i)
		{
			if(i == 0)
				printf("[%f,", error_rate2[i]);
			else if(i == error_rate1.size() - 1)
				printf("%f]\n", error_rate2[i]);
			else
				printf("%f, ", error_rate2[i]);
		}
	} 





	return 0;
}
