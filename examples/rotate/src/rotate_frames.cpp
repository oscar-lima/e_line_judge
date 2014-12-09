#include <iostream>
#include <opencv2/opencv.hpp>

//this program will rotate the frames 90 degrees

int main(int argc, char* argv[]) 
{
	// Create a window and give it a name.
	std::string windowName = "Rotating frames example";
	cv::namedWindow(windowName, CV_WINDOW_AUTOSIZE);
	
	// Initialize video capture from camera and check if it worked.
	cv::VideoCapture vidCap = cv::VideoCapture("../../../data/video/left.MOV");
	//cv::VideoCapture vidCap = cv::VideoCapture("slow_mo.MOV");
	
	if (!vidCap.isOpened()) 
	{
		std::cerr << "Could not open video capture!" << std::endl;
		return 1;
	}
	
	// The main loop.
	while (true) 
	{
		// Read a video frame.
		cv::Mat frame;
		cv::Mat resize_frame;
		
		vidCap >> frame;
		
		//preprocessing the video : 1.rotate the video: transpose, flip and 2. scale
		cv::transpose(frame, frame);
		cv::flip(frame, frame, 1);
		cv::resize(frame, frame, cv::Size(360, 640));
		
		cv::imshow(windowName, frame);
		
		// Quit the loop when a key is pressed.
		int keyPressed = cv::waitKey(10);
		if (keyPressed != -1) break;
	}
	return 0;
}
