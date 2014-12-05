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
         *
         * @return
         * -1 if ball is on the left of the line
         *  0 if ball is on the line
         *  1 if ball is on the right of the line
         */
        int getDecision(const cv::Mat &image, const cv::Point2i &ball_center, double ball_radius);

    private:
        /**
         * @param bottom_point
         * End of the line closer to the bottom of the image
         *
         * @param top_point
         * End of the line closer to the top of the image
         *
         * @param point
         * Point whose position w.r.t the line is checked
         *
         * @return true if point is on the right of the line
         *         false otherwise
         *
         */
        bool isRight(const cv::Point2i &bottom_point, const cv::Point2i &top_point, const cv::Point2i &point);

    private:
        /**
         * Corner points of the line (with thickness, represented as a quadrilateral)
         * Points in order are bottom-left, top-left, top-right, bottom-right
         */
        const std::vector<cv::Point2i> &line_corners_;

};
#endif
