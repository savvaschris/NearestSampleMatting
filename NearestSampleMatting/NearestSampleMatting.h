//NearestSampleMatting.h
//Nearest sample matting algorithm header file
//Author: Savvas Christodoulou
//Contact: schris@cs.ucy.ac.cy
//Cyprus, June 2014

#ifdef NEARESTSAMPLEMATTING_EXPORTS
#define NEARESTSAMPLEMATTING_API __declspec(dllexport) 
#else
#define NEARESTSAMPLEMATTING_API __declspec(dllimport) 
#endif

#include <opencv2\core\core.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <iostream>

#define GLOBAL_SAMPLING 1
#define WINDOW_SAMPLING 0
#define UNKNOWN 128
#define FOREGROUND 255
#define BACKGROUND 0

//Tunable parameters
#define NUMBER_OF_SAMPLES 100
#define GLOBAL_SAMPLING_DENSITY 5
#define FEATURE_RGBXY 1


// This class is exported from the NearestSampleMatting.dll
class NearestSampleMatting{

public: 
    // Return alpha matte
    NEARESTSAMPLEMATTING_API cv::Mat NearestSampleMatting::alpha_matting(
		cv::Mat image, 
		cv::Mat trimap,
		bool sampling_method
	);

	//Create trimap from depth-map
	NEARESTSAMPLEMATTING_API cv::Mat create_trimap_from_depth(cv::Mat depth_map, uchar u_size = 13);

private:
	//Global sampling
	static std::vector<cv::Point> NearestSampleMatting::sample_global_train_set(cv::Mat image, cv::Mat trimap);
	//Local sampling
	static std::vector<cv::Point> NearestSampleMatting::sample_train_set(cv::Mat image, cv::Mat trimap, int i, int j);
	//Best F and B estimation
	static std::vector<cv::Point> NearestSampleMatting::estimate_f_and_b(std::vector<cv::Point> samples, cv::Mat image, cv::Mat trimap, int i, int j);
	//Alpha estimation
	static int NearestSampleMatting::estimate_alpha(std::vector<cv::Point> f_and_b, int i, int j, cv::Mat image);
	//Euclidean distance
	static float NearestSampleMatting::Euclidean(std::vector<int> sample, std::vector<int> unknown);
};
