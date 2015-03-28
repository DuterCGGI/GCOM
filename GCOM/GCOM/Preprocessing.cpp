#include "ImgProc.h"

void ImgProc::PreProcessing(const cv::Mat& img,
	                        cv::Mat& edge)
{
    cv::Mat tmp_img;
    cv::Mat img_smooth;
    cv::Mat img_edge;
    double thresh = 0;
    if (img.type() != 0)
    {
        cv::cvtColor(img, tmp_img, cv::COLOR_BGR2GRAY);
    }
    else
    {
        img.copyTo(tmp_img);
    }
    //smooth the img to remove noise
    cv::GaussianBlur(tmp_img, img_smooth, cv::Size(9,9), 1.5, 2);
    //binarize the image, "canny" from bw image can reduce a lot of noises
    cv::threshold(img_smooth, img_edge, thresh, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    //get the edge map from the bw image
    cv::Canny(img_edge, edge, thresh, 0.4 * thresh, 3);
}


