#include <e_line_judge/pre_processing.h>
#include <opencv2/opencv.hpp>

PreProcessing::PreProcessing()
{
	
}

PreProcessing::~PreProcessing()
{
	
}

void PreProcessing::rotateImage(cv::Mat &image, int width, int height)
{
	cv::transpose(image, image);
    cv::flip(image, image, 1);
	cv::resize(image, image, cv::Size(width, height));
}
