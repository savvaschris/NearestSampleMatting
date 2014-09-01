//NearestSampleMattingApp.cpp
//Nearest sample matting demo
//Author: Savvas Christodoulou
//Contact: schris@cs.ucy.ac.cy
//Cyprus, June 2014

#include <iostream>
#include <opencv2\highgui\highgui.hpp>
#include "NearestSampleMatting.h" 

using namespace std;

void image_matting_demos();

int main()
{
	NearestSampleMatting nsm;

	/*Image with user trimap example*/
	//Create alpha matte
	cv::imwrite(
		"donkey_matte.png",
		 nsm.alpha_matting(
			cv::imread("donkey.png"), 
			cv::imread("donkey_trimap.png",0), 
			WINDOW_SAMPLING
		)
	);

	/*Kinect high resolution image with thresholded depth-map example*/
	//Get thresholded depth-map 
	cv::Mat depth_map = cv::imread("DTH08.png",0);
	//Resize depth-map to match high resolution Kinect image
	cv::resize(depth_map,depth_map,cv::Size(depth_map.cols*2,depth_map.rows*2));
	//Create trimap from depth
	cv::Mat trimap = nsm.create_trimap_from_depth(depth_map);
	//Create alpha matte
	cv::Mat matte = nsm.alpha_matting(cv::imread("IMG08.png"), trimap, WINDOW_SAMPLING);
	//Post processing
	cv::medianBlur(matte, matte, 5);
	cv::imwrite("IMG08_matte.png", matte);

    return 0;
}

