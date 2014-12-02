#include <e_line_judge/ball_tracking.h>

BallTracking::BallTracking(BallDetection *ball_detector, int max_trajectory_length)
    : ball_detector_(ball_detector), max_trajectory_length_(max_trajectory_length)
{

}

BallTracking::~BallTracking()
{
}

void BallTracking::nextImage(const cv::Mat &image)
{
}
