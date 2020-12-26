
#ifndef __DISTANCE_SCALER_REF_H_
#define __DISTANCE_SCALER_REF_H_

#include "core/reference.h"
#include "distance_scaler.h"

namespace Ign
{

class DistanceScalerRef: public Reference
{
	GDCLASS( DistanceScalerRef, Reference );
	OBJ_CATEGORY("Ignition");

protected:
	static void _bind_methods();

public:
	DistanceScalerRef();
	virtual ~DistanceScalerRef();

	void set_plain_distance( real_t dist );
	real_t get_plain_distance() const;

	void set_log_scale( real_t scale );
	real_t get_log_scale() const;

	real_t scale( real_t d ) const;

public:
	DistanceScaler scaler;
};

}





#endif



