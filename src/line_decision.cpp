#include <e_line_judge/line_decision.h>

LineDecision::LineDecision(const std::vector<cv::Point2i> &line_corners) : line_corners_(line_corners)
{
    
}

LineDecision::~LineDecision()
{
}

int LineDecision::getDecision(const cv::Mat &image, double ball_center, double ball_radius)
{
    return 1;
}
