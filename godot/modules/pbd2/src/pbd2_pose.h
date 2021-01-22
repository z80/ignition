
#ifndef __PBD2_POSE_H_
#define __PBD2_POSE_H_

#include "vector3d.h"
#include "quaterniond.h"

using namespace Ign;

namespace Pbd
{

class Pose
{
public:
    Quaterniond q;
    Vector3d    r;

    Pose();
    ~Pose();
    Pose( const Pose & inst );
    const Pose & operator=( const Pose & inst );
    Pose inverse() const;
};

Pose operator*( const Pose & a, const Pose & b );
Pose operator/( const Pose & a, const Pose & b );

}


#endif



