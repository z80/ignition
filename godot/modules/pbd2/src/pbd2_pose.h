
#ifndef __PBD2_POSE_H_
#define __PBD2_POSE_H_

#include "vector3d.h"
#include "quaternion.h"

namespace Pbd
{

class Pose
{
public:
    Quatenriond q;
    Vector3d    r;

    Pose();
    ~Pose();
    Pose( cosnt Pose & inst );
    const Pose & operator=( Pose & inst );
};

}


#endif



