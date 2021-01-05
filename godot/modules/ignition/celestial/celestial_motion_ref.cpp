
#include "celestial_motion_ref.h"

namespace Ign
{

void CelestialMotionRef::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("init", "gm", "se3"), &CelestialMotionRef::init );
	ClassDB::bind_method( D_METHOD("init_gm", "radius_km", "suface_orbit_velocity_kms"), &CelestialMotionRef::init_gm );
	ClassDB::bind_method( D_METHOD("launch_elliptic", "gm", "unit_r", "unit_v", "period_hrs", "eccentricity"), &CelestialMotionRef::launch_elliptic );
	ClassDB::bind_method( D_METHOD("process", "dt"), &CelestialMotionRef::process, Variant::OBJECT );
}

CelestialMotionRef::CelestialMotionRef()
	: Reference()
{
}

CelestialMotionRef::~CelestialMotionRef()
{
}

void CelestialMotionRef::init( real_t gm, const Ref<Se3Ref> & se3 )
{
	cm.init( gm, se3->se3 );
}

real_t CelestialMotionRef::init_gm( real_t radius_km, real_t wanted_surface_orbit_velocity_kms ) const
{
	const Float gm = cm.init_gm( radius_km, wanted_surface_orbit_velocity_kms );
	return gm;
}

void CelestialMotionRef::launch_elliptic( real_t gm, const Vector3 & unit_r, const Vector3 & unit_v, real_t period_hrs, real_t eccentricity )
{
	cm.launch_elliptic( gm, Vector3d(unit_r.x, unit_r.y, unit_r.z), Vector3d(unit_v.x, unit_v.y, unit_v.z), period_hrs, eccentricity );
}

Ref<Se3Ref> CelestialMotionRef::process( real_t dt )
{
	Ref<Se3Ref> se3;
	se3.instance();
	se3->se3 = cm.process( dt );
	return se3;
}


}




