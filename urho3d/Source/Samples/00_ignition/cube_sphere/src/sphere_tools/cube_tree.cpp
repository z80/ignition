
#include "cube_tree.h"

namespace Ign
{

Plane::Plane()
{
    r[0] = 0.0;
    r[1] = 0.0;
    r[2] = 0.0;
    a[0] = 0.0;
    a[1] = 0.0;
    a[2] = 1.0;
}

Plane::~Plane()
{

}

Plane::Plane( const Plane & inst )
{
    *this = inst;
}

const Plane & Plane::operator=( const Plane & inst )
{
    if ( this != &inst )
    {
        r = inst.r;
        a = inst.a;
    }
    return *this;
}

bool Plane::inFront( const Vector3d & t ) const
{
    const Vector3d dr = t - r;
    const Float d = dr.DotProduct( a );
    const bool res = ( d > 0.0 );
    return res;
}





}


