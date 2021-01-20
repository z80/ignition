
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

Pose Pose::operator*( const Pose & b )
{
    Pose ret;
    ret.q = q * b.q;
    ret.r = r + (q*b.r);

    return ret;
}

Pose Pose::operator/( const Pose & b )
{
    Pose inv = b.inverse();
    Pose ret = inv * (*this);

    return ret;
}

Pose Pose::inverse() const
{
    Pose ret;
    ret.q = q.Inverse();
    ret.r = -(ret.q*r);

    return ret;
}



}

