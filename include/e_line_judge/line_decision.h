#ifndef LINE_DECISION_H_
#define LINE_DECISION_H_

#include <opencv2/core/core.hpp>
class LineDecision
{
    public:
        /**
         * @param line_corners
         * Corners of the line in pixel points
         */
        LineDecision(const std::vector<cv::Point2i> &line_corners);
        virtual ~LineDecision();

        /**
         * @param image
         * Image in which to make decision on whether ball is on left, on or to the left of the line
         * @param ball_center
         * Center of the ball (BallDetection or BallTracking provides this)
         *
         * @param ball_radius
         * Radius of the ball (BallDetection or BallTracking provides this)
         */
        int getDecision(const cv::Mat &image, double ball_center, double ball_radius);

    private:
        const std::vector<cv::Point2i> &line_corners_;
};
#endif
