#ifndef SURFACE_CONTACT_DETECTION_H_
#define SURFACE_CONTACT_DETECTION_H_

#include <opencv2/core/core.hpp>
#include <queue>

class SurfaceContactDetection
{
    public:
        SurfaceContactDetection();
        virtual ~SurfaceContactDetection();

        /**
         * @param trajectory
         * Trajectory of the ball in the last x frames (pop should return oldest frame)
         *
         * @return
         * -1 if ball is on the left of the line
         *  0 if ball is on the line
         *  1 if ball is on the right of the line
         */
        int getTrajectoryChangeIndex(const std::queue<cv::Point2i> &trajectory);

};

#endif
