
#ifndef __DISTANCE_SCALER_BASE_REF_H_
#define __DISTANCE_SCALER_BASE_REF_H_

#include "core/object/ref_counted.h"
#include "distance_scaler_base.h"

namespace Ign
{

class DistanceScalerBaseRef: public RefCounted
{
    GDCLASS( DistanceScalerBaseRef, RefCounted );
    //OBJ_CATEGORY("Ignition");

protected:
    static void _bind_methods();

public:
    DistanceScalerBaseRef();
    virtual ~DistanceScalerBaseRef();

    void set_plain_distance( real_t dist );
    real_t get_plain_distance() const;

    real_t scale( real_t d ) const;
    Vector3 scale_v( const Vector3 & v ) const;

    Vector3 unscale_v( const Vector3 & v ) const;

public:
    DistanceScalerBase * scaler_base;
};

}





#endif



