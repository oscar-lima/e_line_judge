#ifndef USER_CALIBRATION_H_
#define USER_CALIBRATION_H_

#include <opencv2/core/core.hpp>

class UserCalibration
{
    public:
        UserCalibration();
        virtual ~UserCalibration();
        /**
         * Asks user to specify region of interest in image with the ball and calculates
         * HSV range for the ball colour
         *
         * @param image
         * Image in which the ball can be seen
         * 
         * @param lower_range
         * Lower range of HSV values representing the ball colour
         * This variable is modified in place
         *
         * @param upper_range
         * Upper range of HSV values representing the ball colour
         * This variable is modified in place
         * 
         */
        void getBallHSVRange(const cv::Mat &image, cv::Scalar &lower_range, cv::Scalar &upper_range);

        /**
         * Asks user to specify the limits of the line (by four mouse clicks) and returns the points
         * 
         * @param image
         * Image in which the line can be seen
         *
         * @param line_corner_points
         * Vector of points on the image representing bottom-left, top-left, top-right, bottom-right
         * corners of the line.
         * This variable is modified in place
         */
        void getLineLimits(const cv::Mat &image, std::vector<cv::Point2i> &line_corner_points);

};
#endif
