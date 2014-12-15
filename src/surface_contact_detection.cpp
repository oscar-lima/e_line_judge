#include <e_line_judge/surface_contact_detection.h>

SurfaceContactDetection::SurfaceContactDetection()
{
}

SurfaceContactDetection::~SurfaceContactDetection()
{
}

bool SurfaceContactDetection::hasTrayectoryChanged(int previous_y_position, int current_y_position)
{    
    if((previous_y_position - current_y_position) > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}
