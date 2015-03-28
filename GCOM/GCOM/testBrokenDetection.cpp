#include "TEST.h"

//img is the global variable
cv::Mat img;
//mouse points on the image
std::vector<cv::Point2f> pts;
//name of the window
const char* window = "Detection";


// on_mouse tracker
// save current point to pts when left button down
void on_mouse(int event, int x, int y, int flags, void* param)  
{  
	static CvPoint tmp_pts = cv::Point(0, 0);  

	switch (event)  
	{  
	case CV_EVENT_LBUTTONDOWN:  
		tmp_pts = cv::Point(x, y);
		pts.push_back(tmp_pts);
		cv::circle(img, tmp_pts, 1, cv::Scalar(0,0,255), 2);
		cv::imshow(window, img);
		break;   
	}  
}  


// showing data
void ShowResult(cv::Mat& img, std::vector<cv::Point>& broken_position,
	            std::vector<cv::Point>& dual_position, 
	            std::vector<cv::Point>& crack_position,
	            const int win_size)
{
	/////circle and rectangle the img for showing//////
	for (size_t i = 0; i < dual_position.size(); ++i)
	{
		int x = dual_position.at(i).x;
		int y = dual_position.at(i).y;
		cv::rectangle(img, cv::Rect(x - win_size/2, y - win_size/2, win_size, win_size), cv::Scalar(0, 0, 255));
		cv::circle(img, dual_position.at(i), 5, cv::Scalar(0,0,255), 2);
	}
	for (size_t i = 0; i < crack_position.size(); ++i)
	{
		int x = crack_position.at(i).x;
		int y = crack_position.at(i).y;
		cv::rectangle(img, cv::Rect(x - win_size/2, y - win_size/2, win_size, win_size), cv::Scalar(255, 255, 0));
		cv::circle(img, crack_position.at(i), 5, cv::Scalar(255,255,0), 2);
	}
	for (size_t i = 0; i < broken_position.size(); ++i)
	{
		int x = broken_position.at(i).x;
		int y = broken_position.at(i).y;
		cv::rectangle(img, cv::Rect(x - win_size/2, y - win_size/2, win_size, win_size), cv::Scalar(255, 0, 0));
		cv::circle(img, broken_position.at(i), 5, cv::Scalar(255,0,0), 2);
	}
	cv::imshow(window, img);
}



void TEST::testBrokenDetection()
{
	//const char* filename = "data/test1.jpg";
	const char* filename = "data/000.bmp";
	/////!!!!!!!!!!!noticed that the ground-truth is 25% the scale of input image////
	img = cv::imread(filename);
	cv::Mat img_edge;
	double thresh = 0;
	std::vector<cv::Point> broken_position;
	std::vector<cv::Point> dual_position;
	std::vector<cv::Point> crack_position;
	int win_size = 40;

	// image pre-processing
	cv::resize(img,img,cv::Size(img.size().width / 2, img.size().height / 2));  //image resize
	ImgProc::PreProcessing(img, img_edge);

	/////////////////////////////////////
	cv::namedWindow(window);
	cv::imshow(window, img);
	cv::setMouseCallback(window, on_mouse, 0);

	int c;  
	do{  
		c = cvWaitKey(0);  
		switch ((char)c)  
		{   
		case 'r':  
			img = cv::imread(filename);
			cv::resize(img,img,cv::Size(img.size().width / 2, img.size().height / 2));  //image resize
			ImgProc::PreProcessing(img, img_edge);
			cv::imshow(window, img);
			break;
		case 'd':
			if (pts.size() == 2)
			{
				cv::line(img, pts[0], pts[1], cv::Scalar(0,255,0), 2);
				cv::line(img_edge, pts[0], pts[1],cv::Scalar(255, 0, 0), 1);
				cv::Vec4f lines;
				cv::fitLine(cv::Mat(pts), lines, CV_DIST_L2, 0, 0.01, 0.01);
				ImgProc::BrokenAreaDetectionNew(img_edge, lines, broken_position, dual_position, crack_position, win_size);
				ShowResult(img, broken_position,dual_position, crack_position,win_size);
				pts.clear();
				broken_position.clear();
				dual_position.clear();
				crack_position.clear();
				break;
			}
			break;
		}  
	}while (c > 0 && c != 27);
}