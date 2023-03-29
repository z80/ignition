
#include "distance_scaler_base.h"
#include <cmath>


namespace Ign
{

DistanceScalerBase::DistanceScalerBase()
{
    plain_dist = 100.0;
}

DistanceScalerBase::~DistanceScalerBase()
{
}

void DistanceScalerBase::set_plain_distance( Float dist )
{
    plain_dist = dist;
}

Float DistanceScalerBase::plain_distance() const
{
    return plain_dist;
}

Float DistanceScalerBase::scale( Float dist ) const
{
    return dist;
}

Vector3d DistanceScalerBase::scale( const Vector3d & v ) const
{
    return v;
}

Vector3d DistanceScalerBase::unscale( const Vector3d & v ) const
{
    return v;
}


}





