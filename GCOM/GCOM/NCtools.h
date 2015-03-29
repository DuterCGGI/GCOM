#ifndef NCTOOLS_H
#define NCTOOLS_H

#include "ConfigCV.h"
#define PI 3.1415926

struct Data
{
	double toolLen;       //���ϳ���
	double toolD;         //ϳ��ֱ��
	double cutEdgelen;    //�����г���
	double taperAgl;      //׶�ȽǶ�
	double helixAgl;      //������
	double precision;     //����
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