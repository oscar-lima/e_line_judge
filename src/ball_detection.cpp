#include <e_line_judge/ball_detection.h>

BallDetection::BallDetection(const cv::Scalar &lower_range, const cv::Scalar &upper_range)
{
	hsv_min = lower_range;
	hsv_max = upper_range;
	std::cout << "Start detection system" << std::endl;
}

BallDetection::~BallDetection()
{
	
}

bool BallDetection::detect_ball(const cv::Mat &image, cv::Point2i &ball_center, double &ball_radius)
{
	cv::Mat hsv_image;
	
	//vector for storing the detected circles
	std::vector<cv::Vec3f> circles;
	
	//convert image to HSV format
	cv::cvtColor(image, hsv_image, CV_BGR2HSV, 0);
	
	//orange color detection
	cv::inRange(hsv_image, hsv_min, hsv_max, hsv_image);
	
	//filter noise
	cv::GaussianBlur(hsv_image, hsv_image, cv::Size(9, 9), 2, 2 );
	
	//Apply the Hough Transform to find the circles
	cv::HoughCircles(hsv_image, circles, CV_HOUGH_GRADIENT, 2, hsv_image.rows/4, 100, 40, 20, 200);
	
	//Detect the largest circle
	double max_radius = 0.0;
	if(circles.size() == 0)
	{
		return false;
	}
	else
	{
		for(size_t i = 0; i < circles.size(); i++)
		{
			//getting x and y of the circle
			cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
			
			//getting the circle radius
			double radius = circles[i][2];
			
			if(radius > max_radius)
			{
				max_radius = radius;
				ball_radius = radius;
				ball_center = center;
			}
		}
		
		return true;
	}

}
