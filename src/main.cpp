#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <e_line_judge/user_calibration.h>
#include <e_line_judge/ball_detection.h>
#include <e_line_judge/ball_tracking.h>
#include <e_line_judge/line_decision.h>
#include <iostream>

cv::Point2i ball_center;

void mouseCallback(int event, int x, int y, int flags, void *param)
{
    if (event == CV_EVENT_LBUTTONDOWN)
    {
        ball_center.x = x;
        ball_center.y = y;
    }
}

int main(int argc, char** argv)
{
    UserCalibration uc;
    cv::Mat image;
    image = cv::imread("../images/big_yellow_ball.jpg");
    cv::Scalar lower_range;
    cv::Scalar upper_range;
    uc.getBallHSVRange(image, lower_range, upper_range);
    std::vector<cv::Point2i> lines;
    uc.getLineLimits(image, lines);

    cv::Mat img_copy;
    image.copyTo(img_copy);
    cv::line(img_copy, lines[0], lines[1], cv::Scalar(0,0,255));
    cv::line(img_copy, lines[1], lines[2], cv::Scalar(0,0,255));
    cv::line(img_copy, lines[2], lines[3], cv::Scalar(0,0,255));
    cv::line(img_copy, lines[3], lines[0], cv::Scalar(0,0,255));

    cv::imshow("Lines", img_copy);
    cv::setMouseCallback("Lines", mouseCallback);
    LineDecision ld(lines);

    while(cv::waitKey(0) != 27)
    {
        int isRight = ld.getDecision(image, ball_center, 0.3);    
        if (isRight == -1) std::cout << "Left" << std::endl;
        else if (isRight == 0) std::cout << "On Line" << std::endl;
        else std::cout << "Right" << std::endl;
    }


    
    return 0;
}
