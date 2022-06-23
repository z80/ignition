
#ifndef __DISTANCE_SCALER_REF_H_
#define __DISTANCE_SCALER_REF_H_

#include "core/reference.h"
#include "distance_scaler_base_ref.h"
#include "distance_scaler.h"

namespace Ign
{

class DistanceScalerRef: public DistanceScalerBaseRef
{
	GDCLASS( DistanceScalerRef, DistanceScalerBaseRef );
	OBJ_CATEGORY("Ignition");

protected:
	static void _bind_methods();

public:
	DistanceScalerRef();
	virtual ~DistanceScalerRef();

	void set_log_scale( real_t scale );
	real_t get_log_scale() const;

public:
	DistanceScaler scaler;
};

}





#endif



