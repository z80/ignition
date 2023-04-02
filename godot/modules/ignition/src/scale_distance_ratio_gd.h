
#ifndef __SCALE_DISTANCE_RATIO_GD_H_
#define __SCALE_DISTANCE_RATIO_GD_H_

#include "core/object/ref_counted.h"
#include "scale_distance_ratio.h"
#include "se3_ref.h"

namespace Ign
{

class ScaleDistanceRatioGd: public RefCounted
{
	GDCLASS( ScaleDistanceRatioGd, RefCounted );
	//OBJ_CATEGORY("Ignition");

protected:
	static void _bind_methods();

public:
	ScaleDistanceRatioGd();
	virtual ~ScaleDistanceRatioGd();

	void set_max_distance( real_t dist );
	real_t get_max_distance() const;

	real_t compute_scale( const Ref<Se3Ref> & rel_se3, real_t base_scale ) const;
	Transform3D compute_transform( const Ref<Se3Ref> & rel_se3, real_t base_scale ) const;

public:
	ScaleDistanceRatio ratio;
};

}






#endif


