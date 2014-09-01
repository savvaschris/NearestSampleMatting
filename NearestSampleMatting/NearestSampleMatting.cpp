//NearestSampleMatting.cpp 
//Nearest sample matting algorithm source file
//Author: Savvas Christodoulou
//Contact: schris@cs.ucy.ac.cy
//Cyprus, June 2014

#include "NearestSampleMatting.h"

using namespace std;


//Alpha matting algorithm
//Parameters: image, trimap, sampling method
//Returns: alpha matte
cv::Mat NearestSampleMatting::alpha_matting(cv::Mat image, cv::Mat trimap, bool sampling_method){

	
	cv::Mat alpha_matte = cv::Mat::zeros(image.rows, image.cols, CV_8UC1);
	std::vector<cv::Point> samples;

	//Global sampling scheme
	if(sampling_method == GLOBAL_SAMPLING){
		samples = sample_global_train_set(image, trimap);
	}

	//For each unknown pixel estimate alpha
	for(int i = 0; i < trimap.rows; i++){
		for(int j = 0; j < trimap.cols; j++){
			if(trimap.at<uchar>(i,j) == UNKNOWN){
				
				//Local sampling scheme
				if(sampling_method == WINDOW_SAMPLING){
					samples = sample_train_set(image, trimap, i, j);
				}

				//Estimate closest F and B
				std::vector<cv::Point> f_and_b;
				f_and_b = estimate_f_and_b(samples, image, trimap, i, j);	

				//Estimate alpha
				alpha_matte.at<uchar>(i,j) = estimate_alpha(f_and_b, i, j, image);
			
			}
			else if(trimap.at<uchar>(i,j) == FOREGROUND){
				alpha_matte.at<uchar>(i,j) = FOREGROUND;
			}
			else if(trimap.at<uchar>(i,j) == BACKGROUND){
				alpha_matte.at<uchar>(i,j) = BACKGROUND;
			}
			else { std::cout<<"Error in trimap\n"; }
		}
	}

	return alpha_matte;
}

//Global sampling
//Parameters: image and trimap
//Returns: samples collected from image
std::vector<cv::Point> NearestSampleMatting::sample_global_train_set(cv::Mat image, cv::Mat trimap){
	
	std::vector<cv::Point> samples;
	
	//Sample all definite foreground and definite background pixels in image
	for(int m = 0; m < trimap.rows; m+=GLOBAL_SAMPLING_DENSITY){
		for(int n = 0; n < trimap.cols; n+=GLOBAL_SAMPLING_DENSITY){
			if(trimap.at<uchar>(m,n) == FOREGROUND){
				cv::Point sample; 
				sample.x = m; sample.y = n;
				samples.push_back(sample);
			}
			if(trimap.at<uchar>(m,n) == BACKGROUND){
				cv::Point sample; 
				sample.x = m; sample.y = n;
				samples.push_back(sample);
			}
		}
	}

	return samples;
}

//Local sampling
//Parameters: image and trimap
//Returns: samples collected from image
std::vector<cv::Point> NearestSampleMatting::sample_train_set(cv::Mat image, cv::Mat trimap, int i, int j){

	std::vector<cv::Point> samples;
	int smpl_num = NUMBER_OF_SAMPLES, fore_smpl_num = 0, back_smpl_num = 0,
		start_row = i-1, end_row = i+1, start_col = j-1, end_col = j+1;
	//Sample using all contents of window
	do{
		for(int m = start_row; m <= end_row; m++){
			for(int n = start_col; n <= end_col; n++){
				if(((m == start_row) || (m == end_row) || (n == start_col) || (n == end_col))
					&& ((m >= 0) && (m < trimap.rows) && (n >= 0) && (n < trimap.cols))){
					
					if(trimap.at<uchar>(m,n) == FOREGROUND){
						cv::Point sample; 
						sample.x = m; sample.y = n;
						samples.push_back(sample);
						fore_smpl_num++;
					}
					if(trimap.at<uchar>(m,n) == BACKGROUND){
						cv::Point sample; 
						sample.x = m; sample.y = n;
						samples.push_back(sample);
						back_smpl_num++;
					}
				}
			}
		}

		start_row--; end_row++;	start_col--; end_col++;
	}while((fore_smpl_num < smpl_num) || (back_smpl_num < smpl_num));
		
	return samples;
}

//Best F and B estimation
//Parameters: samples collected, image, trimap, unknown pixel x and y coordinates
//Returns: estimated foreground and background colors
std::vector<cv::Point> NearestSampleMatting::estimate_f_and_b(std::vector<cv::Point> samples, cv::Mat image, cv::Mat trimap, int i, int j){

	std::vector<cv::Point> f_and_b;
	std::vector<int>background_sample, foreground_sample, testData;
	cv::Point nearest_back, nearest_fore;
	float distance_back = 9999, distance_fore = 9999;

	//Unknown pixel
	testData.push_back(image.at<cv::Vec3b>(i, j)[0]);
	testData.push_back(image.at<cv::Vec3b>(i, j)[1]);
	testData.push_back(image.at<cv::Vec3b>(i, j)[2]);
	if(FEATURE_RGBXY){
		testData.push_back(i);
		testData.push_back(j);
	}

	//Get nearest using Euclidean distance
	for (int s=0; s<(int)samples.size(); s++){

		if (trimap.at<uchar>(samples.at(s).x, samples.at(s).y) == BACKGROUND){
						
			background_sample.push_back(image.at<cv::Vec3b>(samples.at(s).x, samples.at(s).y)[0]);
			background_sample.push_back(image.at<cv::Vec3b>(samples.at(s).x, samples.at(s).y)[1]);
			background_sample.push_back(image.at<cv::Vec3b>(samples.at(s).x, samples.at(s).y)[2]);
			if(FEATURE_RGBXY){
				background_sample.push_back(samples.at(s).x);
				background_sample.push_back(samples.at(s).y);
			}
			float dist = Euclidean(background_sample, testData);
						
			if(dist < distance_back){
				
				distance_back = dist;
				nearest_back.x = samples.at(s).x;
				nearest_back.y = samples.at(s).y;
			}
			background_sample.clear();
		}
		else 
		if (trimap.at<uchar>(samples.at(s).x, samples.at(s).y) == FOREGROUND){

			foreground_sample.push_back(image.at<cv::Vec3b>(samples.at(s).x, samples.at(s).y)[0]);
			foreground_sample.push_back(image.at<cv::Vec3b>(samples.at(s).x, samples.at(s).y)[1]);
			foreground_sample.push_back(image.at<cv::Vec3b>(samples.at(s).x, samples.at(s).y)[2]);
			if(FEATURE_RGBXY){
				foreground_sample.push_back(samples.at(s).x);
				foreground_sample.push_back(samples.at(s).y);
			}
			int dist = Euclidean(foreground_sample, testData);
			if(dist < distance_fore){
				
				distance_fore = dist;
				nearest_fore.x = samples.at(s).x;
				nearest_fore.y = samples.at(s).y;
			}	
			foreground_sample.clear();
		}
	}

	f_and_b.push_back(nearest_fore);
	f_and_b.push_back(nearest_back);
	
	return f_and_b;
}

//Alpha estimation
//Parameters: estimated foreground and background position, unknown pixel x and y coordinates, image
//Returns: estimated alpha value 
int NearestSampleMatting::estimate_alpha(std::vector<cv::Point> f_and_b, int i, int j, cv::Mat image){

	std::vector<float> CB, FB, F, B, C;

	//Foreground
	F.push_back(image.at<cv::Vec3b>(f_and_b.at(0).x, f_and_b.at(0).y)[0]);
	F.push_back(image.at<cv::Vec3b>(f_and_b.at(0).x, f_and_b.at(0).y)[1]);
	F.push_back(image.at<cv::Vec3b>(f_and_b.at(0).x, f_and_b.at(0).y)[2]);

	//Background
	B.push_back(image.at<cv::Vec3b>(f_and_b.at(1).x, f_and_b.at(1).y)[0]);
	B.push_back(image.at<cv::Vec3b>(f_and_b.at(1).x, f_and_b.at(1).y)[1]);
	B.push_back(image.at<cv::Vec3b>(f_and_b.at(1).x, f_and_b.at(1).y)[2]);

	//Unknown
	C.push_back(image.at<cv::Vec3b>(i, j)[0]);
	C.push_back(image.at<cv::Vec3b>(i, j)[1]);
	C.push_back(image.at<cv::Vec3b>(i, j)[2]);

	//Calculate alpha with matting equation
	for (int k=0; k<F.size(); k++){
	
		CB.push_back((C.at(k)-B.at(k)));
		FB.push_back((F.at(k)-B.at(k)));
	}

	float resultN=0, resultD=0;
	for (int k=0; k<F.size(); k++){
		
		resultN+=CB.at(k)*FB.at(k);
		resultD+=FB.at(k)*FB.at(k);
	}
	
	int alpha = (resultN/resultD)*255;
	if(alpha > 255)
		return 255;
	else
	if (alpha < 0)
		return 0;
	else
		return alpha;
}

//Euclidean distance
//Parameters: sample and unkown pixels feature vectors
//Returns: distance
float NearestSampleMatting::Euclidean(std::vector<int> sample, std::vector<int> unknown){
	
	float euclid = 0;
	for (int i = 0; i < sample.size(); i++){
	
		float value1 = sample.at(i);
		float value2 = unknown.at(i);

		euclid = euclid + pow((value1 - value2), 2);
	}
	
	return sqrt(euclid) / sample.size();
}

//Create trimap from depth-map
//Parameters: thresholded depth-map
//Returns: trimap
cv::Mat NearestSampleMatting::create_trimap_from_depth(cv::Mat depth_map, uchar u_size){

	cv::Mat trimap(depth_map.rows, depth_map.cols, CV_8UC1);
	uchar m_size = 5;

	cv::blur(depth_map, trimap, cv::Size(m_size, m_size));
	cv::threshold(trimap, trimap, 50, 255, CV_THRESH_BINARY);
	cv::blur(trimap, trimap, cv::Size(u_size, u_size));

	for(int i=0; i < depth_map.rows; i++){
	
		for(int j=0; j < depth_map.cols; j++){
			
			if(trimap.at<uchar>(i,j) > BACKGROUND && trimap.at<uchar>(i,j) < FOREGROUND)
				trimap.at<uchar>(i,j) = UNKNOWN;
		}
	}

	return trimap;
}


