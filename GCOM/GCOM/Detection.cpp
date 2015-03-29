
#include "ImgProc.h"
/*              introduction about this algorithm                                                  */
/* this algorithm only consider the standard line with large slope or general slope,  maybe it 
* would collapse while the slope is near zero but it can be handll by some trick anyway.
*/



/*                              procedure of the algorithm                                         */

/*
1.  supposed that the curcature of the edge for our experiment is usually convex, thus we can use the start and end point of the std_line
to calculate a threshold for discriminate large slope and general slope
2.  if it is a general slope , we can calculate the distance from any point to std_line by the formula below:
abs(A*x0 + B*y0 + C)/sqrt(A*A + B*B)
(1) we use mean and standard variance for detecting the broken area
(2) while for detecting the dual-area,we use the the fact that two slope with perpendicularity which their product are -1.
Since there might be a inaccuracy, we use a low threshold for discriminating.
(3) for detectiong the crack-area, we simply count its non-zero value in the detect window, and using a proper threshold can be efficient.

*/





void ImgProc::BrokenAreaDetectionNew(const cv::Mat& img_edge, const cv::Vec4f& lines, 
                                     std::vector<cv::Point>& broken_position, 
                                     std::vector<cv::Point>& dual_position, 
                                     std::vector<cv::Point>& crack_position, 
                                     const int& win_size)
{
    //handle input error
    if (img_edge.type() != 0)
    {
        return;
    }

    double slope_threshold = 50.0; // if any slope is larger than this threshold , regard its slope  as infinity.

    //track_path is the path for searching-windows
    int rows = img_edge.rows;
    int cols = img_edge.cols;
    std::vector<cv::Point> track_path;

    cv::Point start, end;
    start.x = int((lines[0]/lines[1]) * (0.0 - lines[3]) + lines[2]);
    start.y = int(0);
    end.x = int((lines[0]/lines[1]) * (rows - lines[3]) + lines[2]);
    end.y = rows;


    /// this loop is to find the tracking path of the standard image
    /// determin the sigh of the slope
    int sigh;

    if (lines[1]/lines[0]>0)
    {
        sigh = 1;
    }
    else
    {
        sigh = -1;
    }

    int it = 1;
    while(1)
    {
        if (start.x+lines[0]*it*win_size*sigh<0)
            break;
        if (start.x+lines[0]*it*win_size*sigh>cols)
            break;
        if (start.y+lines[1]*it*win_size*sigh<0)
            break;
        if (start.y+lines[1]*it*win_size*sigh>rows)
            break;
        track_path.push_back(cv::Point(int(start.x+lines[0]*it*win_size*sigh), 
			                           int(start.y+lines[1]*it*win_size*sigh) ));
        ++it;
    }
    //handle error when the track_path is empty
    if (track_path.size()==0)
    {
        return;
    }

    std::vector<cv::Point>::const_iterator iter = track_path.begin();
    int index = win_size;
    double diff_y_judge = (double)(track_path.begin()->y - (track_path.end() - 1)->y);
    double diff_x_judge = (double)(track_path.begin()->x - (track_path.end() - 1)->x);
    double slope_judge = diff_y_judge / diff_x_judge;
    for (; iter < track_path.end(); ++iter)
    {

        //double slope = (double)(iter->y - ( iter + win_size )->y) / (double)(iter->x - ( iter + win_size)->x );
        double slope = lines[1] / lines[0];
        cv::Mat distance_array;
        std::vector<cv::Point> negative_cor;
        std::vector<cv::Point> positive_cor;

        for (int i = iter->y - win_size / 2; i <= iter->y + win_size / 2; ++i) 
        {
            for (int j = iter->x - win_size / 2; j <= iter->x + win_size / 2; ++j) 
            {
                if (i < 0 || j < 0 ||
					i >= rows || j >= cols)
                {
                    continue;
                }
                else
                {
                    if (int(img_edge.at<uchar>(i, j)) != 0)
                    {
                        if ( slope_judge <  slope_threshold ) 
                        {
                            double current_distance = (double(std::abs(i - slope * j + slope * iter->x - iter->y) / std::sqrt(1 + slope * slope)));
                            distance_array.push_back<double>(current_distance);
                            if ((i - slope * j + slope * iter->x - iter->y) > 0) 
                            {
                                positive_cor.push_back(cv::Point(j, i));
                            }
                            else 
                            {
                                negative_cor.push_back(cv::Point(j, i));
                            }
                        }
                        else
                        {
                            double current_distance = double(std::abs(i - iter->y));
                            distance_array.push_back<double>(current_distance);
                            if (double(i - iter->y) > 0) 
                            {
                                positive_cor.push_back(cv::Point(j, i));
                            } else 
                            {
                                negative_cor.push_back(cv::Point(j, i));
                            }

                        }
                    }
                }
            }
        }

        /// detecting broken area by standard variance, empirically set the threshold to 8.0

        cv::Mat mean_distance, std_distance;

        if (distance_array.size().width != 0) {

            /// broken area detection
            cv::meanStdDev(distance_array, mean_distance, std_distance);
            if (slope_judge <  slope_threshold) 
            {
                if (std_distance.at<double>(0) >= 3 && mean_distance.at<double>(0) >= 5) {               
                    broken_position.push_back(cv::Point(iter->x, iter->y));
                }
            }
            else
            {
                if (std_distance.at<double>(0) >= 3 && mean_distance.at<double>(0) >= 5) {
                    broken_position.push_back(cv::Point(iter->x, iter->y));
                }
            }

            ///dual area detection, version 2
            int dual_index = 0;
            if (positive_cor.size() > 0 && negative_cor.size() > 0 &&distance_array.size().height!=0)
            {
                if (slope_judge <  slope_threshold) 
                {
                    for (size_t i = 0; i < positive_cor.size(); ++i) 
                    {
                        for (size_t j = 0; j < negative_cor.size(); ++j) 
                        {
                            double diff_y = (double) (positive_cor.at(i).y - negative_cor.at(j).y);
                            double diff_x = (double) (positive_cor.at(i).x - negative_cor.at(j).x);
                            double slope_check = diff_y / diff_x;
                            if (std::abs(slope_check - (-1.0 / slope)) < 0.12) 
                            {
                                ++dual_index;
                                continue;
                            }
                        }
                    }
                }
                else
                {
                    for (size_t i = 0; i < positive_cor.size() - 1; ++i) 
                    {
                        for (size_t j = 0; j < negative_cor.size() - 1; ++j)
                        {
                            double diff_y = (double) (positive_cor.at(i).y - negative_cor.at(j).y);                           
                            if (diff_y == 0)
                            {
                                ++dual_index;
                                continue;
                            }
                        }
                    }
                }
                if (dual_index > win_size) 
                {
                    dual_position.push_back(cv::Point(iter->x, iter->y));
                }
            }

            ///crack area detection
            if (distance_array.size().height <= win_size/2)
            {
                crack_position.push_back(cv::Point(iter->x, iter->y));
            }
        }
        else
        {
            crack_position.push_back(cv::Point(iter->x, iter->y));
        }
    }
}





