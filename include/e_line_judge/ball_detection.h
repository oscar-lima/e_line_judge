#ifndef BALL_DETECTION_H_
#define BALL_DETECTION_H_

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

class BallDetection
{
    public:
        /**
         * @param lower_range
         * Lower range of HSV values defining the colour of the ball
         *
         * @param upper_range
         * Upper range of HSV values defining the colour of the ball
         */
        BallDetection(const cv::Scalar &lower_range, const cv::Scalar &upper_range, double radius_estimate);

        virtual ~BallDetection();

        /**
         * @param image
         * Image in which to detect the ball
         *
         * @param ball_center
         * Center of the detected ball
         *
         * @param radius
         * Radius of the detected ball
         *
         * @return true if ball has been detected
         */
		
        bool detect_ball(const cv::Mat &image, cv::Point2i &ball_center, double &radius, bool debug=false);        
	
	private:
	
		cv::Scalar hsv_min, hsv_max;
        double radius_estimate_;

};
#endif

