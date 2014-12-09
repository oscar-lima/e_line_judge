#ifndef PRE_PROCESSING_H_
#define PRE_PROCESSING_H_

#include <opencv2/core/core.hpp>

class PreProcessing
{
    public:
        /**
         * empty constructor
         */
        PreProcessing();
        virtual ~PreProcessing();
        /**
         * @param image
		 * image : source frame to be rotated (90 degrees clockwise
		 * 
		 * @param width
		 * width: the image will be resized to this width value after rotation
		 * 
		 * @param heigh
		 * height: the image will be resized to this height value after rotation
		 * 
         */
        void rotateImage(cv::Mat &image, int width, int height);

    private:
        
};

#endif
