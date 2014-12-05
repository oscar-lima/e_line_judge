#include <e_line_judge/user_calibration.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

void UserCalibration::mouseCallback(int event, int x, int y, int flags, void *param)
{
    UserCalibration *this_obj = static_cast<UserCalibration*>(param);
    this_obj->doMouseCallback(event, x, y, flags);
}

UserCalibration::UserCalibration() : set_center(false), set_radius(false), set_bottom_left(false), set_top_left(false),
                                     set_top_right(false), set_bottom_right(false)
{
}

UserCalibration::~UserCalibration()
{

}
void UserCalibration::doMouseCallback(int event, int x, int y, int flags)
{
    if (event == CV_EVENT_LBUTTONDOWN)
    {
        if(!set_center)
        {
            ball_center.x = x;
            ball_center.y = y;
            set_center = true;
        }
        else if (!set_radius)
        {
            ball_radius = sqrt(pow(y - ball_center.y, 2) + pow(x - ball_center.x, 2));
            set_radius = true;
        }
        else if (!set_bottom_left)
        {
            bottom_left.x = x;
            bottom_left.y = y;
            set_bottom_left = true;            
        }
        else if (!set_top_left)
        {
            top_left.x = x;
            top_left.y = y;
            set_top_left = true;
        }
        else if (!set_top_right)
        {
            top_right.x = x;
            top_right.y = y;
            set_top_right = true;
        }
        else
        {
            bottom_right.x = x;
            bottom_right.y = y;
            set_bottom_right = true;
        }
    }
}

void UserCalibration::getBallHSVRange(const cv::Mat &image, cv::Scalar &lower_range, cv::Scalar &upper_range)
{
    /** waitKey needs to be changed so it waits for a mouse event
     * We also need some type of status window that gives instructions instead of std::cout */
    cv::namedWindow("Ball Calibration", cv::WINDOW_AUTOSIZE);
    cv::setMouseCallback("Ball Calibration", mouseCallback, this);
    cv::imshow("Ball Calibration", image);

    std::cout << "Click center of the ball" << std::endl;
    cv::waitKey(0);
    std::cout << "Click any edge of the ball" << std::endl;
    cv::waitKey(0);

    cv::destroyWindow("Ball Calibration");

    /** Maybe loop here asking the user whether they are satisfied with the circle */
    cv::Mat display_image;
    image.copyTo(display_image);
    cv::circle(display_image, ball_center, ball_radius, cv::Scalar(0,0,255));
    cv::imshow("Circle", display_image);
    cv::waitKey(0);

    cv::destroyWindow("Circle");

    cv::Mat empty_image = cv::Mat::zeros(image.rows, image.cols, CV_8UC1);
    cv::circle(empty_image, ball_center, ball_radius, cv::Scalar(255,255,255), -1);
    cv::Mat region_of_interest;
    cv::bitwise_and(image, image, region_of_interest, empty_image);
    cv::imshow("roi", region_of_interest);
    cv::waitKey(0);
    cv::destroyWindow("roi");
    
    cv::Mat hsv_image;
    cv::cvtColor(region_of_interest, hsv_image, CV_BGR2HSV);
    cv::imshow("hsvroi", hsv_image);
    cv::waitKey(0);
    cv::destroyWindow("hsvroi");


    int min_h = 256;
    int min_s = 256;
    int min_v = 256;
    int max_h = -1;
    int max_s = -1;
    int max_v = -1;
   
    /* loop ROI and get min and max HSV values */

    for (int i = 0; i < hsv_image.rows; i++)
    {
        for (int j = 0; j < hsv_image.cols; j++)
        {
            cv::Vec3b hsv = hsv_image.at<cv::Vec3b>(i,j);
            if (hsv.val[2] != 0) // if not black  (possible point of failure - what if ball is black?)
            {
                if (hsv.val[0] < min_h) min_h = hsv.val[0];
                if (hsv.val[1] < min_s) min_s = hsv.val[1];
                if (hsv.val[2] < min_v) min_v = hsv.val[2];

                if (hsv.val[0] > max_h) max_h = hsv.val[0];
                if (hsv.val[1] > max_s) max_s = hsv.val[1];
                if (hsv.val[2] > max_v) max_v = hsv.val[2];
            }
        }
    }
    lower_range = cv::Scalar(min_h, min_s, min_v);
    upper_range = cv::Scalar(max_h, max_s, max_v);
    std::cout << "Range: " << lower_range <<", " << upper_range << std::endl;
}

void UserCalibration::getLineLimits(const cv::Mat &image, std::vector<cv::Point2i> &line_corner_points)
{
    cv::namedWindow("Line Calibration", cv::WINDOW_AUTOSIZE);
    cv::setMouseCallback("Line Calibration", mouseCallback, this);
    cv::imshow("Line Calibration", image);

    std::cout << "Click bottom left corner of line" << std::endl;
    cv::waitKey(0);
    std::cout << "Click top left corner of line" << std::endl;
    cv::waitKey(0);
    std::cout << "Click top right corner of line" << std::endl;
    cv::waitKey(0);
    std::cout << "Click bottom right corner of line" << std::endl;
    cv::waitKey(0);
    cv::destroyWindow("Line Calibration");

    // TODO: actually check that these points are set
    line_corner_points.push_back(bottom_left);
    line_corner_points.push_back(top_left);
    line_corner_points.push_back(top_right);
    line_corner_points.push_back(bottom_right);

    cv::Mat display_image;
    image.copyTo(display_image);
    cv::line(display_image, bottom_left, top_left, cv::Scalar(0,0,255));
    cv::line(display_image, top_left, top_right, cv::Scalar(0,0,255));
    cv::line(display_image, top_right, bottom_right, cv::Scalar(0,0,255));
    cv::line(display_image, bottom_right, bottom_left, cv::Scalar(0,0,255));
    cv::imshow("lines", display_image);
    cv::waitKey(0);
    cv::destroyWindow("lines");
}
