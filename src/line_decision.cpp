#include <e_line_judge/line_decision.h>

LineDecision::LineDecision(const std::vector<cv::Point2i> &line_corners) : line_corners_(line_corners)
{
    
}

LineDecision::~LineDecision()
{
}
bool LineDecision::isRight(const cv::Point2i &bottom_point, const cv::Point2i &top_point, const cv::Point2i &point)
{
    if ((bottom_point.x - top_point.x) * (point.y - top_point.y) - 
        (bottom_point.y - top_point.y) * (point.x - top_point.x) < 0)
        return true;
    return false;
}
int LineDecision::getDecision(const cv::Mat &image, const cv::Point2i &ball_center, double ball_radius)
{
    if (!isRight(line_corners_.at(0), line_corners_.at(1), ball_center))
    {
        return -1; //left of line          
    }
    else if (isRight(line_corners_.at(3), line_corners_.at(2), ball_center))
    {
        return 1; //right of the line
    }
    else 
    {
        return 0; // on the line
    }
}
