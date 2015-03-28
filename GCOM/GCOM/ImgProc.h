#ifndef IMGPROC_H
#define IMGPROC_H

#include "ConfigCV.h"

class ImgProc
{
public:
	static void PreProcessing(const cv::Mat& img, 
		                      cv::Mat& edge);
	static void BrokenAreaDetectionNew(const cv::Mat& img_edge, const cv::Vec4f& lines, 
		                               std::vector<cv::Point>& broken_position, 
		                               std::vector<cv::Point>& dual_position, 
		                               std::vector<cv::Point>& crack_position, 
		                               const int& win_size);
};

#endif