
#ifndef __DISTANCE_SCALER_H_
#define __DISTANCE_SCALER_H_

#include "distance_scaler_base.h"
#include "data_types.h"
#include "vector3d.h"

namespace Ign
{

class DistanceScaler: public DistanceScalerBase
{
public:
    DistanceScaler();
    virtual ~DistanceScaler();

    void set_log_scale( Float scale );
    Float log_scale() const;

	virtual Float scale( Float dist ) const override;
	virtual Vector3d scale( const Vector3d & v ) const override;

	virtual Vector3d unscale( const Vector3d & v ) const override;

public:
    Float log_scale_;
};


}


#endif




