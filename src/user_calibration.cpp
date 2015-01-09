#include <e_line_judge/user_calibration.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <unistd.h>
#include <iostream>

void UserCalibration::mouseCallback(int event, int x, int y, int flags, void *param)
{
    UserCalibration *this_obj = static_cast<UserCalibration*>(param);
    this_obj->doMouseCallback(event, x, y, flags);
}

void UserCalibration::displayMouseCallback(int event, int x, int y, int flags, void *param)
{
    UserCalibration *this_obj = static_cast<UserCalibration*>(param);
    this_obj->doDisplayMouseCallback(event, x, y, flags);
}

UserCalibration::UserCalibration() : set_center(false), set_radius(false), set_bottom_left(false), set_top_left(false),
                                     set_top_right(false), set_bottom_right(false), answer_received(false), is_satisfied(false)
{
    display = cv::Mat::zeros(250,250,CV_8UC3);
    cv::imshow("Display", display);
    cv::setMouseCallback("Display", displayMouseCallback, this);
}

UserCalibration::~UserCalibration()
{    
}

void UserCalibration::doDisplayMouseCallback(int event, int x, int y, int flags)
{
    if (event == CV_EVENT_LBUTTONDOWN)
    {
        if (!answer_received)
        {
            if (x > 10 && x < 100 && y > 30 && y < 45)
            {
                is_satisfied = true; 
                answer_received = true;
            }
            if (x > 150 && x < 240 && y > 30 && y < 45)
            {
                is_satisfied = false;
                answer_received = true;
            }
        }
    }
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

void UserCalibration::getBallHSVRange(const cv::Mat &image, cv::Scalar &lower_range, cv::Scalar &upper_range, double &radius, bool debug)
{
    /** waitKey needs to be changed so it waits for a mouse event
     * We also need some type of status window that gives instructions instead of std::cout */
    cv::namedWindow("Ball Calibration", cv::WINDOW_AUTOSIZE);
    cv::setMouseCallback("Ball Calibration", mouseCallback, this);
    cv::imshow("Ball Calibration", image);
    
    bool satisfied = false;
    
    std::cout << "Starting ball calibration" << std::endl;    
    
    do
    {
        setDisplay("Click center of the ball");
        while (!set_center)
        {
            cv::waitKey(30);
        }
        setDisplay("Click any edge of the ball");
        while(!set_radius)
        {
            cv::waitKey(30);
        }
        
        //drawing circle around the clicked ball
        /** Maybe loop here asking the user whether they are satisfied with the circle */
        cv::Mat display_image;
        image.copyTo(display_image);
        cv::circle(display_image, ball_center, ball_radius, cv::Scalar(0,0,255));
        cv::imshow("Circle", display_image);
        cv::waitKey(30);
        
        setDisplayQuestion("Is the circle fully inside the ball?", "yes", "no");
        answer_received = false;
        while(!answer_received)
        {
            cv::waitKey(30);
        }
        /*
        //checkin with user if he is satisfied with the clicked region
        char answer = 'n';
        std::cout << "Is the circle fully inside the ball? (n/y) : " << std::endl;
        try
        {
            std::cin >> answer;
            std::cout << "Answer was : " << answer << std::endl;
        }
        catch(...)
        {
            std::cout << "Error while reading answer" << std::endl;
        }
        
        if(answer == 'y')
        {
            satisfied = true;
        }
        else
        {
            satisfied = false;
            set_center = false;
            set_radius = false;
        } */
        if (is_satisfied)
        {
            satisfied = true;
        }
        else
        {
            satisfied = false;
            set_center = false;
            set_radius = false;
            answer_received = false;
        }
        cv::destroyWindow("Circle");
    }
    while(!satisfied);

    radius = ball_radius; 
    
    cv::destroyWindow("Ball Calibration");
    cv::Mat empty_image = cv::Mat::zeros(image.rows, image.cols, CV_8UC1);
    cv::circle(empty_image, ball_center, ball_radius, cv::Scalar(255,255,255), -1);
    cv::Mat region_of_interest;
    cv::bitwise_and(image, image, region_of_interest, empty_image);
    
    if(debug)
    {
        cv::imshow("roi", region_of_interest);
        cv::waitKey(0);
        cv::destroyWindow("roi");
    }
    
    cv::Mat hsv_image;
    cv::cvtColor(region_of_interest, hsv_image, CV_BGR2HSV);
    
    if(debug)
    {
        cv::imshow("hsvroi", hsv_image);
        cv::waitKey(0);
        cv::destroyWindow("hsvroi");
    }

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
    std::cout << "Radius: " << radius << std::endl;
}

int UserCalibration::getBallFrameEstimate()
{
    int frame_number = 0;
    
    std::cout << "Please provide with a frame estimate (number) in which the ball appears in the FOV of the camera" << std::endl;
    try
    {
        std::cin >> frame_number;
        std::cout << "Skipping to frame number : " << frame_number << std::endl;
        return frame_number;
    }
    catch(...)
    {
        std::cout << "Error while reading input integer" << std::endl;
        return 0;
    }
}

void UserCalibration::getLineLimits(const cv::Mat &image, std::vector<cv::Point2i> &line_corner_points)
{
    cv::namedWindow("Line Calibration", cv::WINDOW_AUTOSIZE);
    cv::setMouseCallback("Line Calibration", mouseCallback, this);
    cv::imshow("Line Calibration", image);

    std::cout << "Starting line calibration" << std::endl;
    bool satisfied =  false;
    
    do
    {
        line_corner_points.clear();
        setDisplay("Click bottom left corner of line");
        while(!set_bottom_left)
        {
            cv::waitKey(30);
        }
        setDisplay("Click top left corner of line");
        while(!set_top_left)
        {
            cv::waitKey(30);
        }
        setDisplay("Click top right corner of line");
        while(!set_top_right)
        {
            cv::waitKey(30);
        }
        setDisplay("Click bottom right corner of line");
        while(!set_bottom_right)
        {
            cv::waitKey(30);
        }

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
        cv::waitKey(30);
        setDisplayQuestion("Are you satisfied with the calibration?", "yes", "no");
        answer_received = false;
        while(!answer_received)
        {
            cv::waitKey(30);
        }
        if (is_satisfied)
        {
            satisfied = true;
        }
        else
        {
            satisfied = false;
            set_bottom_left = false;
            set_top_left = false;
            set_top_right = false;
            set_bottom_right = false;
            answer_received = false;
        }
        cv::destroyWindow("lines");
    }while(!satisfied);
    cv::destroyWindow("Line Calibration");
    setDisplay("Calibration done");
}

void UserCalibration::setDisplay(const std::string &text)
{    
    display = cv::Mat::zeros(50,300,CV_8UC3);
    cv::putText(display, text, cv::Point(5,25), CV_FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,0,255),1,8,false);
    cv::imshow("Display", display);
}

void UserCalibration::setDisplayQuestion(const std::string &text, const std::string &option1, const std::string &option2)
{
    display = cv::Mat::zeros(50,350,CV_8UC3);
    cv::putText(display, text, cv::Point(5,25), CV_FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,0,255),1,8,false);
    cv::putText(display, option1, cv::Point(30,40), CV_FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,0,255),1,8,false);
    cv::putText(display, option2, cv::Point(170,40), CV_FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,0,255),1,8,false);
    cv::rectangle(display, cv::Point(10,30), cv::Point(100,45), cv::Scalar(0,0,255),1);
    cv::rectangle(display, cv::Point(150,30), cv::Point(240,45), cv::Scalar(0,0,255),1);
    cv::imshow("Display", display);
}
