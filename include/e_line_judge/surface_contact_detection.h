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
         * @param previous_y_position
         * Previous ball y center of the ball values expressed in pixels
		 * 
		 * @param current_y_position
		 * Current ball y center of the ball value expressed in pixels
         *
         * @return boolean that states if there is a change in the trajectory
         */
        bool hasTrayectoryChanged(int previous_y_position, int current_y_position);

};

#endif
