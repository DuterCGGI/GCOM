
#include<iostream>
#include<fstream>
#include<string>
#include "NCtools.h"

using namespace std;

void NCtools::toolcircle(double mea_pos,
	                     double rev_spe, 
					     int frame,
					     const char* filename)
{

	//const char * file = "toolcircle.txt";

	double measuring_position = -mea_pos;
	double rotation_speed = 45 * frame;

	ofstream fout(filename, ios_base::out | ios_base::trunc);

	fout << "N10 G91" << endl;
	fout << "N20 G00 X" << -mea_pos << endl;
	fout << "N30 G01 A360 F" << 45 * frame << endl;
	fout << "N40 G00 X" << mea_pos << " A" << -360 << endl;
	fout << "N50 M30" << endl;
	fout.close();
} 
void NCtools::toolcutlen(double helixAgl,
	                     double toolR,
					     double cutEdgelen,
					     int frame,
				  	     const char* filename)
{

	//const char * file = "toolcutlen.txt";

	double rotation_angle = (cutEdgelen * tan(helixAgl)) / 
		                    (2 * PI * toolR) * 360;
	double rotation_speed = 45 * frame;

	ofstream fout(filename, ios_base::out | ios_base::trunc);

	fout << "N10 G91" << endl;
	fout << "N20 #5=0" << endl;
	fout << "N30 WHILE[[#5*90] LT 360] DO" << endl;
	fout << "N40 G00 A90" << endl;
	fout << "N50 G01 X" << -cutEdgelen 
		 << " A" << rotation_angle 
		 << " F" << rotation_speed << endl;
	fout << "N60 G00 X" << cutEdgelen 
		 << " A" << -rotation_angle << endl;
	fout << "N70 #5 = #5 + 1" << endl;
	fout << "N80 ENDWHILE" << endl;
	fout << "N90 G00 A-360" << endl;
	fout << "N90 M30" << endl;
	fout.close();
}

void NCtools::toolEdgecircle(double mea_pos,
	                         double toolR, 
						     int frame,
						     const char* filename)
{
	//const char * file = "toolEdgecircle.txt";

	double measuring_position = -mea_pos;
	double rotation_speed = 45 * frame;
	
	ofstream fout(filename, ios_base::out | ios_base::trunc);

	fout << "N10 G91" << endl;
	fout << "N20 G00 X" << mea_pos 
		 << " Y" << -toolR 
		 << " Z" << -toolR << endl;
	fout << "N30 G01 A360 F" << rotation_speed << endl;
	fout << "N40 G00 X" << -mea_pos 
		 << " Y" << toolR 
		 << " Z" << toolR  << endl;
	fout << "N50 M30" << endl;
	fout.close();
}

void NCtools::ProjCurve2Img(Data data, 
	                        const cv::Mat& ProjMat,
	                        std::vector<cv::Point2f>& ImgCoordinate)
{
	//toolLen：棒料长度， toolD：钻头直径， cutEdgelen:切削刃长度
	//taperAgl:锥度角度， helixAgl:螺旋角， precision:精度步长
	double a;
	int n; 
	int i;
	//计算过程中所需的参数，toolR:铣刀半径， lead:导程
	/*Calculate toolmid;*/
	double toolR = data.toolD/2;
	double lead = 2*PI*toolR/tan(data.helixAgl);

	//计算基准线是选取的节点个数
	int numofpoint;
	a = data.cutEdgelen/data.precision;
	if (a-floor(a)<=0.5)
		numofpoint = (int)floor(a);
	else
		numofpoint = (int)ceil(a);

	n = numofpoint;

	//定义螺旋线节点的x,y,z构成的数组

	/*------------------*/
	cv::Mat CurveCoordinate = cv::Mat::zeros(3,1,CV_64F);
	cv::Mat tmp;
	double X,Y;
	double mtoolR, mc;
	for(i=0;i<n+1;i++)
	{
		/*------------------------*/
		CurveCoordinate.at<double>(0) = 0-(double)i/(double)n * data.cutEdgelen;
		mtoolR = -CurveCoordinate.at<double>(0)*tan(data.taperAgl*PI/360);
		mc = -2*PI*CurveCoordinate.at<double>(0) / lead;
		CurveCoordinate.at<double>(1) = (toolR+mtoolR)*sin(mc);
		CurveCoordinate.at<double>(2) = (toolR+mtoolR)*cos(mc);
		/*-----------------------*/
		tmp = ProjMat * CurveCoordinate;
		X = tmp.at<double>(0) / 
			tmp.at<double>(2);
		Y = tmp.at<double>(1) / 
			tmp.at<double>(2);
		if (X > 0 && X < 1&& 
			Y > 0 && Y < 1){
				ImgCoordinate.push_back(cv::Point2f((float)X,(float)Y));
		}
	}
}