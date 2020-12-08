
#include "pbd2_pose.h"

namespace Pbd
{

Pose::Pose()
{
}

Pose::~Pose()
{
}

Pose::Pose( cosnt Pose & inst )
{
    *this = inst;
}

const Pose & Pose::operator=( Pose & inst )
{
    if ( this != &inst )
    {
        q = inst.q;
        r = inst.r;
    }
}


}

