
#include "celestial_motion_ref.h"
#include "ref_frame_node.h"

namespace Ign
{

void CelestialMotionRef::_bind_methods()
{
    ClassDB::bind_method( D_METHOD("set_allow_orbiting", "en"), &CelestialMotionRef::set_allow_orbiting );
    ClassDB::bind_method( D_METHOD("get_allow_orbiting"), &CelestialMotionRef::get_allow_orbiting, Variant::BOOL );

    ClassDB::bind_method( D_METHOD("set_stationary_threshold", "th"), &CelestialMotionRef::set_stationary_threshold );
    ClassDB::bind_method( D_METHOD("get_stationary_threshold"), &CelestialMotionRef::get_stationary_threshold, Variant::REAL );

    ClassDB::bind_method( D_METHOD("stop"), &CelestialMotionRef::stop );

    ClassDB::bind_method( D_METHOD("init", "gm", "se3"), &CelestialMotionRef::init );
    ClassDB::bind_method( D_METHOD("init_gm", "radius_km", "suface_orbit_velocity_kms"), &CelestialMotionRef::init_gm );
    ClassDB::bind_method( D_METHOD("launch_elliptic", "gm", "unit_r", "unit_v", "period_hrs", "eccentricity"), &CelestialMotionRef::launch_elliptic );
    ClassDB::bind_method( D_METHOD("process", "dt"), &CelestialMotionRef::process, Variant::OBJECT );
    ClassDB::bind_method( D_METHOD("process_rf", "dt", "rf"), &CelestialMotionRef::process_rf );
    ClassDB::bind_method( D_METHOD("duplicate"), &CelestialMotionRef::duplicate, Variant::OBJECT );


    ADD_PROPERTY( PropertyInfo( Variant::BOOL, "allow_orbiting" ),       "set_allow_orbiting",       "get_allow_orbiting" );
    ADD_PROPERTY( PropertyInfo( Variant::REAL, "stationary_threshold" ), "set_stationary_threshold", "get_stationary_threshold" );

}

CelestialMotionRef::CelestialMotionRef()
        : Reference()
{
}

CelestialMotionRef::~CelestialMotionRef()
{
}

void CelestialMotionRef::set_allow_orbiting( bool en )
{
    cm.set_allow_orbiting( en );
}

bool CelestialMotionRef::get_allow_orbiting() const
{
    const bool ret = cm.get_allow_orbiting();
    return ret;
}

void CelestialMotionRef::set_stationary_threshold( real_t th )
{
    cm.set_stationary_threshold( th );
}

real_t CelestialMotionRef::get_stationary_threshold() const
{
    const real_t ret = cm.get_stationary_threshold();
    return ret;
}

void CelestialMotionRef::stop()
{
    cm.stop();
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

void CelestialMotionRef::process_rf( real_t dt, Node * rf )
{
    RefFrameNode * rf_node = Node::cast_to<RefFrameNode>( rf );
    if (rf_node != nullptr )
        rf_node->se3_ = cm.process( dt );
}


Ref<CelestialMotionRef> CelestialMotionRef::duplicate() const
{
    Ref<CelestialMotionRef> ret;
    ret.instance();
    ret->cm = cm;

    return ret;
}


}




