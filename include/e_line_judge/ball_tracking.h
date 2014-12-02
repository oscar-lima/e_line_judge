#ifndef BALL_TRACKING_H_
#define BALL_TRACKING_H_

#include <opencv2/core/core.hpp>
#include <queue>
#include <e_line_judge/ball_detection.h>

class BallTracking
{
    public:
        /**
         * @param ball_detector
         * Pointer to BallDetection object
         *
         * @param max_trajectory_length
         * Maximum length of trajectory that should be saved
         */
        BallTracking(BallDetection *ball_detector, int max_trajector_length);
        virtual ~BallTracking();
        /**
         * @param image
         * Image in which to find current position of ball
         */
        void nextImage(const cv::Mat &image);

    private:
        std::queue<cv::Point2i> trajectory_points_;
        int max_trajectory_length_;
        BallDetection *ball_detector_;
};

#endif
