
#ifndef __DISTANCE_SCALER_H_
#define __DISTANCE_SCALER_H_

#include "data_types.h"
#include "vector3d.h"

namespace Ign
{

class DistanceScaler
{
public:
	DistanceScaler();
	virtual ~DistanceScaler();

	void set_plain_distance( Float dist );
	Float plain_distance() const;

	void set_log_scale( Float scale );
	Float log_scale() const;

	Float scale( Float dist ) const;
	Vector3d scale( const Vector3d & v ) const;

public:
	Float plain_dist_;
	Float log_scale_;
};


}


#endif




