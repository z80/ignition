
#ifndef __CELESTIAL_MOTION_REF_H_
#define __CELESTIAL_MOTION_REF_H_

#include "core/reference.h"
#include "se3_ref.h"
#include "celestial_motion.h"


namespace Ign
{

class RefFrameNode;

class CelestialMotionRef: public Reference
{
	GDCLASS( CelestialMotionRef, Reference );
	OBJ_CATEGORY("Ignition");

protected:
	static void _bind_methods();

public:
	CelestialMotionRef();
	~CelestialMotionRef();

	void init( real_t gm, const Ref<Se3Ref> & se3 );
	real_t init_gm( real_t radius_km, real_t wanted_surface_orbit_velocity_kms ) const;
	void launch_elliptic( real_t gm, const Vector3 & unit_r, const Vector3 & unit_v, real_t period_hrs, real_t eccentricity );
	Ref<Se3Ref> process( real_t dt );
	void process_rf( real_t dt, Node * rf );

public:
	CelestialMotion cm;
};


}



#endif





