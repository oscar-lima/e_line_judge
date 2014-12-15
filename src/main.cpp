#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <e_line_judge/user_calibration.h>
#include <e_line_judge/ball_detection.h>
#include <e_line_judge/ball_tracking.h>
#include <e_line_judge/line_decision.h>
#include <e_line_judge/pre_processing.h>
#include <e_line_judge/surface_contact_detection.h>
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
    int frame_calibration_number;
    bool slow_motion = false;
    bool live_video = false;
    bool rotate_image = true;
    bool hardcoded_calibration = true;
    
    //reading example image, uncomment for debugging
    //image = cv::imread("../images/big_yellow_ball.jpg");
    
    cv::VideoCapture vidCap;
    
    if(argc == 1)
    {
        std::cout << "Using ball falling on the left video" << std::endl;
        vidCap = cv::VideoCapture("../data/video/left.MOV");
    }
    
    if(argc > 1)
    {
        std::cout << "argv = " << argv[1] << std::endl;
        
        if(!strcmp(argv[1], "left"))
        {
            std::cout << "Using ball falling on the left video" << std::endl;
            vidCap = cv::VideoCapture("../data/video/left.MOV");
        }
        else if(!strcmp(argv[1], "right"))
        {
            std::cout << "Using ball falling on the right video" << std::endl;
            vidCap = cv::VideoCapture("../data/video/right.MOV");
        }
        else if(!strcmp(argv[1], "center"))
        {
            std::cout << "Using ball falling on the center video" << std::endl;
            vidCap = cv::VideoCapture("../data/video/center.MOV");
        }
        else if (!strcmp(argv[1], "live"))
        {
            std::cout << "Using live video" << std::endl;
            vidCap = cv::VideoCapture(1);
            live_video = true;
            rotate_image = false;
        }
        else if(!strcmp(argv[1], "file"))
        {
            if (argc > 2)
            {
                vidCap = cv::VideoCapture(argv[2]);                
                if (!vidCap.isOpened())
                {
                    std::cout << "Cannot open file " << argv[2] << std::endl;
                    exit(0);
                }
            }
            else
            {
                std::cout << "Specify a filename " << std::endl;
                exit(0);
            }
        } 
        if(argc > 3)
        {
            slow_motion = true;
        }
    }
    
    //rotating the image
    PreProcessing pp;
    
    if (!live_video)
    {
        frame_calibration_number = uc.getBallFrameEstimate();
        
        for(int i = 0; i < frame_calibration_number; i++)
        {
            vidCap >> image;
        }
        vidCap.set(CV_CAP_PROP_POS_AVI_RATIO, 0);
    }
    else
    {
        vidCap >> image;
    }

    std::cout << "Video size: " << image.cols << "x" << image.rows << std::endl;
    if (rotate_image)
    {
        std::cout << "Rotating and resizing video to: 360x640" << std::endl;
        pp.rotateImage(image, 360, 640);
    }
    
    std::cout << "Showing calibration frame" << std::endl;
    
    //get HSV range for the ball to calibrate
    cv::Scalar lower_range;
    cv::Scalar upper_range;
    double radius_estimate;
    if (!hardcoded_calibration)
    {
        uc.getBallHSVRange(image, lower_range, upper_range, radius_estimate, false);
    }
    else
    {
        lower_range = cv::Scalar(12, 119, 200, 0);
        upper_range = cv::Scalar(18, 249, 255, 0);
        radius_estimate = 10.0;
    }
    
    //get line coordinates
    std::vector<cv::Point2i> lines;
    uc.getLineLimits(image, lines);

    //Displaying the input lines
    /*
    cv::Mat img_copy;
    image.copyTo(img_copy);
    cv::line(img_copy, lines[0], lines[1], cv::Scalar(0,0,255));
    cv::line(img_copy, lines[1], lines[2], cv::Scalar(0,0,255));
    cv::line(img_copy, lines[2], lines[3], cv::Scalar(0,0,255));
    cv::line(img_copy, lines[3], lines[0], cv::Scalar(0,0,255));
    cv::imshow("Lines", img_copy);
    */

    uc.setDisplay("Starting Electronic Line Judge...");

    //tracking the ball
    //restarting the video capture to frame 0
    
    //creating instance of tracking class
    BallDetection bd(lower_range, upper_range, radius_estimate);
    cv::Point2i ball_center;
    LineDecision ld(lines);
    int decision_result = 0;
    double ball_radius;
    
    std::queue<cv::Point2i> ball_center_queue;
    SurfaceContactDetection scd;
    cv::Point2i previous_ball_center(0.0, 0.0);
    int ball_detection_iteration = 0;

    bool made_decision = false;
    
    while (true) 
    {
        // Read a video frame
        vidCap >> image;

        if(!image.empty())
        {
            //rotating frame
            if (rotate_image)
            {
                pp.rotateImage(image, 360, 640);
            }
            
            //do stuff
            if(bd.detect_ball(image, ball_center, ball_radius))
            {
                //after detection of the ball, print its value on y
                //std::cout << "ball y coordinate : " << ball_center.y << std::endl;
                
                //drawing detected circles
                // circle center
                cv::circle(image, ball_center, 3, cv::Scalar(0,255,0), -1, 8, 0);
                // circle outline
                cv::circle(image, ball_center, ball_radius, cv::Scalar(0,0,255), 3, 8, 0);
                
                std::cout << "prev: " << previous_ball_center.y << ", curr: " << ball_center.y << std::endl;
                if(!made_decision && ball_detection_iteration != 0 && scd.hasTrayectoryChanged(previous_ball_center.y, ball_center.y))
                {
                    //testing the line decision, uncomment for debugging
                    cv::Point2i projected_ball_center = ball_center;
                    projected_ball_center.y += ball_radius;
                    cv::circle(image, projected_ball_center, 3, cv::Scalar(255,0,0), -1, 8, 0);
                    decision_result = ld.getDecision(image, projected_ball_center, ball_radius);

                    // result image
                    cv::Mat result_image;
                    image.copyTo(result_image);
                    cv::line(result_image, lines[0], lines[1], cv::Scalar(0,0,255));
                    cv::line(result_image, lines[1], lines[2], cv::Scalar(0,0,255));
                    cv::line(result_image, lines[2], lines[3], cv::Scalar(0,0,255));
                    cv::line(result_image, lines[3], lines[0], cv::Scalar(0,0,255));

                    if(decision_result == -1) 
                    {
                        uc.setDisplay("Decision: LEFT");
                        std::cout << "Left" << std::endl;
                        cv::putText(result_image, "LEFT", cv::Point(5,result_image.rows - 20), 
                                    CV_FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0,0,255),1,8,false);
                        if (!live_video)
                        {
                            made_decision = true;
                        }
                    }
                    else if(decision_result == 0)
                    {
                        if (live_video)
                        {
                            uc.setDisplay("Decision: LEFT");
                            cv::putText(result_image, "LEFT", cv::Point(5,result_image.rows - 20), 
                                    CV_FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0,0,255),1,8,false);
                            std::cout << "Left" << std::endl;
                        }
                        else
                        {
                            uc.setDisplay("Decision: ON LINE");
                            cv::putText(result_image, "ON LINE", cv::Point(5,result_image.rows - 20), 
                                    CV_FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0,0,255),1,8,false);
                            std::cout << "On Line" << std::endl;
                        }
                        if (!live_video)
                        {
                            made_decision = true;
                        }
                    }
                    else
                    {
                        uc.setDisplay("Decision: RIGHT");
                        cv::putText(result_image, "RIGHT", cv::Point(5,result_image.rows - 20), 
                                    CV_FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0,0,255),1,8,false);
                        std::cout << "Right" << std::endl;
                        if (!live_video)
                        {
                            made_decision = true;
                        }
                    }
                    cv::imshow("Result", result_image);
                }
                ball_detection_iteration++;
                previous_ball_center = ball_center;
            }
            
            cv::line(image, lines[0], lines[1], cv::Scalar(0,0,255));
            cv::line(image, lines[1], lines[2], cv::Scalar(0,0,255));
            cv::line(image, lines[2], lines[3], cv::Scalar(0,0,255));
            cv::line(image, lines[3], lines[0], cv::Scalar(0,0,255));
            cv::imshow("Ball detection", image);
            
            // Quit the loop when a key is pressed, also give a delay on the video
            int wait_time = 5;
            if (slow_motion)
            {
                wait_time = 500;
            }
            int keyPressed = cv::waitKey(wait_time);
            if (keyPressed != -1) break;
        }
        else
        {
            vidCap.set(CV_CAP_PROP_POS_AVI_RATIO , 0);
            std::cout << "Reached end of video, playing again" << std::endl; 
            uc.setDisplay("Starting Electronic Line Judge...");
            made_decision = false;
            previous_ball_center.x = 0.0;
            previous_ball_center.y = 0.0;
            ball_detection_iteration = 0;
            cv::destroyWindow("Result");
        }
    }
    
    return 0;
}
