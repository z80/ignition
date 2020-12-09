
#include "pbd2_pose.h"

namespace Pbd
{

Pose::Pose()
{
}

Pose::~Pose()
{
}

Pose::Pose( const Pose & inst )
{
    *this = inst;
}

const Pose & Pose::operator=( const Pose & inst )
{
    if ( this != &inst )
    {
        q = inst.q;
        r = inst.r;
    }

    return *this;
}


}

