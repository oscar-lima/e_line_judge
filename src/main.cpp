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
		else
		{
			std::cout << "Using ball falling on the center video" << std::endl;
			vidCap = cv::VideoCapture("../data/video/center.MOV");
		}

        if(argc > 2)
        {
            slow_motion = true;
        }
	}
	
	//rotating the image
	PreProcessing pp;
	
	frame_calibration_number = uc.getBallFrameEstimate();
	
	for(int i = 0; i < frame_calibration_number; i++)
	{
		vidCap >> image;
	}
	
	pp.rotateImage(image, 360, 640);
	
	std::cout << "Showing calibration frame" << std::endl;
	
	//get HSV range for the ball to calibrate
    cv::Scalar lower_range;
    cv::Scalar upper_range;
    uc.getBallHSVRange(image, lower_range, upper_range, false);
	
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
	vidCap.set(CV_CAP_PROP_POS_AVI_RATIO , 0);
	
	//creating instance of tracking class
	BallDetection bd(lower_range, upper_range);
	cv::Point2i ball_center;
	LineDecision ld(lines);
	int decision_result = 0;
	double ball_radius;
	
	std::queue<cv::Point2i> ball_center_queue;
	SurfaceContactDetection scd;
	cv::Point2i previous_ball_center(0.0, 0.0); 

    bool made_decision = false;
	
	while (true) 
	{
		// Read a video frame
		vidCap >> image;

		if(!image.empty())
		{
			//rotating frame
			pp.rotateImage(image, 360, 640);
			
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
				
				if(!made_decision && scd.hasTrayectoryChanged(previous_ball_center.y, ball_center.y))
				{
					//testing the line decision, uncomment for debugging
					decision_result = ld.getDecision(image, ball_center, ball_radius);    
					if(decision_result == -1) 
					{
                        uc.setDisplay("Decision: LEFT");
						std::cout << "Left" << std::endl;
                        made_decision = true;
					}
					else if(decision_result == 0)
					{
                        uc.setDisplay("Decision: ON LINE");
						std::cout << "On Line" << std::endl;
                        made_decision = true;
					}
					else
					{
                        uc.setDisplay("Decision: RIGHT");
						std::cout << "Right" << std::endl;
                        made_decision = true;
					}
				}
				previous_ball_center = ball_center;
			}
			
			cv::imshow("Ball detection", image);
			
			// Quit the loop when a key is pressed, also give a delay on the video
            int wait_time = 5;
            if (slow_motion)
            {
                wait_time = 100;
            }
			int keyPressed = cv::waitKey(wait_time);
			if (keyPressed != -1) break;
		}
		else
		{
			vidCap.set(CV_CAP_PROP_POS_AVI_RATIO , 0);
			std::cout << "Reached end of video, playing again" << std::endl; 
            made_decision = false;
		}
	}
	
    return 0;
}
