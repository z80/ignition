
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

Pose Pose::inverse() const
{
    Pose ret;
    ret.q = q.Inverse();
    ret.r = -(ret.q*r);

    return ret;
}


Pose operator*( const Pose & a, const Pose & b )
{
    Pose ret;
    ret.q = a.q * b.q;
    ret.r = a.r + (a.q*b.r);

    return ret;
}

Pose operator/( const Pose & a, const Pose & b )
{
    Pose inv = b.inverse();
    Pose ret = inv * a;

    return ret;
}
}

