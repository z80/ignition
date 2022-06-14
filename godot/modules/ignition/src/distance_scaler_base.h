
#ifndef __DISTANCE_SCALER_BASE_H_
#define __DISTANCE_SCALER_BASE_H_

#include "data_types.h"
#include "vector3d.h"

namespace Ign
{

class DistanceScalerBase
{
public:
    DistanceScalerBase();
    virtual ~DistanceScalerBase();

    void set_plain_distance( Float dist );
    Float plain_distance() const;

    virtual Float scale( Float dist ) const;
    virtual Vector3d scale( const Vector3d & v ) const;

    virtual Vector3d unscale( const Vector3d & v ) const;

public:
    Float plain_dist;
};


}


#endif




