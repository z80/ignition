
#ifndef __SCALE_DISTANCE_RATIO_H_
#define __SCALE_DISTANCE_RATIO_H_

#include "data_types.h"
#include "se3.h"

namespace Ign
{

class ScaleDistanceRatio
{
public:
	ScaleDistanceRatio();
	~ScaleDistanceRatio();

	void set_max_distance( Float dist );
	Float get_max_distance() const;

	Float compute_scale( const SE3 & rel_se3, Float base_scale ) const;
	Float compute_scale( const Vector3d & at, Float base_scale ) const;
	Float compute_scale( const Float dist, Float base_scale ) const;

public:
	Float max_distance;
};

}


#endif


