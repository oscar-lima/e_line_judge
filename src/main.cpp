#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <e_line_judge/user_calibration.h>
#include <e_line_judge/ball_detection.h>
#include <e_line_judge/ball_tracking.h>
#include <e_line_judge/line_decision.h>
#include <iostream>

int main(int argc, char** argv)
{
    UserCalibration uc;
    cv::Mat image;
    image = cv::imread("../images/big_yellow_ball.jpg");
    cv::Scalar lower_range;
    cv::Scalar upper_range;
    uc.getBallHSVRange(image, lower_range, upper_range);
    std::vector<cv::Point2i> line_corner_points;
    uc.getLineLimits(image, line_corner_points);
    for (int i = 0; i < line_corner_points.size(); i++)
    {
        std::cout << line_corner_points.at(i).x << ", " << line_corner_points.at(i).y << std::endl;
    }
    return 0;
}
