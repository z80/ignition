
#ifndef __SCALE_DISTANCE_RATIO_H_
#define __SCALE_DISTANCE_RATIO_H_

#include "core/reference.h"
#include "distance_scaler_base.h"

namespace Ign
{

class ScaleDistanceRatioGd: public Reference
{
	GDCLASS( ScaleDistanceRatioRef, Reference );
	OBJ_CATEGORY("Ignition");

protected:
	static void _bind_methods();

public:
	ScaleDistanceRatioGd();
	virtual ~ScaleDistanceRatioGd();

	void set_max_distance( real_t dist );
	real_t get_max_distance() const;

	Transform compute_transform( const Ref<Se3Ref> & rel_se3, real_t base_scale ) const;

public:
	ScaleDistanceRatio ratio;
};

}






#endif


