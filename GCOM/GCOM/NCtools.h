#ifndef NCTOOLS_H
#define NCTOOLS_H

#include "ConfigCV.h"
#define PI 3.1415926

struct Data
{
	double toolLen;       //棒料长度
	double toolD;         //铣刀直径
	double cutEdgelen;    //切削刃长度
	double taperAgl;      //锥度角度
	double helixAgl;      //螺旋角
	double precision;     //精度
};


class NCtools
{
public:
	static void toolcircle(double mea_pos, 
		                   double rev_spe, 
				           int frame,
					       const char* filename);
	static void toolcutlen(double helixAgl,
		                   double toolR, 
				           double cutEdgelen, 
					       int frame,
					       const char* filename);
	static void toolEdgecircle(double mea_pos,
		                       double toolR, 
				       		   int frame,
						       const char* filename);
	static void ProjCurve2Img(Data data, 
		                      const cv::Mat& ProjMat,
		                      std::vector<cv::Point2f>& ImgCoordinate);
};

#endif