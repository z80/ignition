
#include "celestial_motion_ref.h"
#include "ref_frame_node.h"
#include "save_load.h"

namespace Ign
{

void CelestialMotionRef::_bind_methods()
{
    ClassDB::bind_method( D_METHOD("set_allow_orbiting", "en"), &CelestialMotionRef::set_allow_orbiting );
    ClassDB::bind_method( D_METHOD("get_allow_orbiting"), &CelestialMotionRef::get_allow_orbiting );

    ClassDB::bind_method( D_METHOD("stop"), &CelestialMotionRef::stop );

    ClassDB::bind_method( D_METHOD("is_orbiting"), &CelestialMotionRef::is_orbiting );
    ClassDB::bind_method( D_METHOD("movement_type"), &CelestialMotionRef::movement_type );
    ClassDB::bind_method( D_METHOD("specific_angular_momentum"), &CelestialMotionRef::specific_angular_momentum );
    ClassDB::bind_method( D_METHOD("eccentricity"), &CelestialMotionRef::eccentricity );
    ClassDB::bind_method( D_METHOD("period"), &CelestialMotionRef::period );
    ClassDB::bind_method( D_METHOD("time_after_periapsis"), &CelestialMotionRef::time_after_periapsis );
    ClassDB::bind_method( D_METHOD("closest_approach"), &CelestialMotionRef::closest_approach );
    ClassDB::bind_method( D_METHOD("perigee"), &CelestialMotionRef::perigee );
    ClassDB::bind_method( D_METHOD("apogee"), &CelestialMotionRef::apogee );
    ClassDB::bind_method( D_METHOD("min_velocity"), &CelestialMotionRef::min_velocity );
    ClassDB::bind_method( D_METHOD("max_velocity"), &CelestialMotionRef::max_velocity );
    ClassDB::bind_method( D_METHOD("excess_velocity"), &CelestialMotionRef::excess_velocity );
    ClassDB::bind_method( D_METHOD("deflection_angle"), &CelestialMotionRef::deflection_angle );
	ClassDB::bind_method( D_METHOD("acceleration"),       &CelestialMotionRef::acceleration );
	ClassDB::bind_method( D_METHOD("ex"), &CelestialMotionRef::ex );
    ClassDB::bind_method( D_METHOD("ey"), &CelestialMotionRef::ey );

    ClassDB::bind_method( D_METHOD("set_se3", "se3"), &CelestialMotionRef::set_se3 );
    ClassDB::bind_method( D_METHOD("get_se3"),        &CelestialMotionRef::get_se3 );

    ClassDB::bind_method( D_METHOD("get_gm"),        &CelestialMotionRef::get_gm );

    ClassDB::bind_method( D_METHOD("launch", "gm", "se3"), &CelestialMotionRef::launch );
    ClassDB::bind_method( D_METHOD("compute_gm_by_speed",  "radius_km", "suface_orbit_velocity_kms"), &CelestialMotionRef::compute_gm_by_speed );
	ClassDB::bind_method( D_METHOD("compute_gm_by_period", "radius_km", "period_kms"), &CelestialMotionRef::compute_gm_by_period );
	ClassDB::bind_method( D_METHOD("launch_elliptic", "gm", "unit_r", "unit_v", "period_hrs", "eccentricity"), &CelestialMotionRef::launch_elliptic );
    ClassDB::bind_method( D_METHOD("process", "dt"), &CelestialMotionRef::process );
    ClassDB::bind_method( D_METHOD("process_rf", "dt", "rf"), &CelestialMotionRef::process_rf );
    ClassDB::bind_method( D_METHOD("duplicate"), &CelestialMotionRef::duplicate );

	ClassDB::bind_method( D_METHOD("orbit_points",  "orbiting_center", "camera_node", "qty", "scale_distance_ratio", "base_scale"), &CelestialMotionRef::orbit_points );

	ClassDB::bind_method( D_METHOD("set_force_numerical", "en"), &CelestialMotionRef::set_force_numerical );
	ClassDB::bind_method( D_METHOD("get_force_numerical"),       &CelestialMotionRef::get_force_numerical );

	ClassDB::bind_method( D_METHOD("set_debug", "en"), &CelestialMotionRef::set_debug );
	ClassDB::bind_method( D_METHOD("get_debug"), &CelestialMotionRef::get_debug );

	ADD_GROUP( "Ignition", "" );
    ADD_PROPERTY( PropertyInfo( Variant::BOOL,   "allow_orbiting" ),       "set_allow_orbiting",       "get_allow_orbiting" );
    ADD_PROPERTY( PropertyInfo( Variant::OBJECT, "se3" ),                  "set_se3",                  "get_se3" );
	ADD_PROPERTY( PropertyInfo( Variant::BOOL,   "force_numerical" ),      "set_force_numerical",      "get_force_numerical" );
	ADD_PROPERTY( PropertyInfo( Variant::BOOL,   "debug" ),                "set_debug",                "get_debug" );
}

CelestialMotionRef::CelestialMotionRef()
        : RefCounted()
{
}

CelestialMotionRef::~CelestialMotionRef()
{
}

void CelestialMotionRef::set_allow_orbiting( bool en )
{
	const bool prev = cm.get_allow_orbiting();
	if ( prev && (!en) )
		print_line( "********************************* CelestialMotionRef: reset allow_orbiting to false!!!" );
    cm.set_allow_orbiting( en );
}

bool CelestialMotionRef::get_allow_orbiting() const
{
    const bool ret = cm.get_allow_orbiting();
    return ret;
}

void CelestialMotionRef::stop()
{
    cm.stop();
}

bool CelestialMotionRef::is_orbiting() const
{
    return cm.is_orbiting();
}

String CelestialMotionRef::movement_type() const
{
    CelestialMotion::Type t = cm.movement_type();
    String ret;
    switch (t)
    {
    case CelestialMotion::NUMERICAL:
        ret = "numeric";
        break;

    case CelestialMotion::ELLIPTIC:
        ret = "elliptic";
        break;

    case CelestialMotion::PARABOLIC:
        ret = "parabolic";
        break;

    case CelestialMotion::HYPERBOLIC:
        ret = "hyperbolic";
		break;

    default:
        ret = "idle";
		break;
    }

    return ret;
}

real_t CelestialMotionRef::specific_angular_momentum() const
{
    const real_t ret = cm.specific_angular_momentum();
    return ret;
}

real_t CelestialMotionRef::eccentricity() const
{
    const real_t ret = cm.eccentricity();
    return ret;
}

real_t CelestialMotionRef::period() const
{
    const real_t ret = cm.period();
    return ret;
}

real_t CelestialMotionRef::time_after_periapsis() const
{
    const real_t ret = cm.time_after_periapsis();
    return ret;
}

real_t CelestialMotionRef::closest_approach() const
{
    const real_t ret = cm.closest_approach();
    return ret;
}

real_t CelestialMotionRef::perigee() const
{
    const real_t ret = cm.perigee();
    return ret;
}

real_t CelestialMotionRef::apogee() const
{
    const real_t ret = cm.apogee();
    return ret;
}

real_t CelestialMotionRef::min_velocity() const
{
    const real_t ret = cm.min_velocity();
    return ret;
}

real_t CelestialMotionRef::max_velocity() const
{
    const real_t ret = cm.max_velocity();
    return ret;
}

real_t CelestialMotionRef::excess_velocity() const
{
    const real_t ret = cm.excess_velocity();
    return ret;
}

real_t CelestialMotionRef::deflection_angle() const
{
    const real_t ret = cm.deflection_angle();
    return ret;
}

Vector3 CelestialMotionRef::acceleration() const
{
	const Vector3d acc_d = cm.acceleration();
	const Vector3 acc( acc_d.x_, acc_d.y_, acc_d.z_ );
	return acc;
}

Vector3 CelestialMotionRef::ex() const
{
    const Vector3d e = cm.ex();
    const Vector3 ret( e.x_, e.y_, e.z_ );
    return ret;
}

Vector3 CelestialMotionRef::ey() const
{
    const Vector3d e = cm.ey();
    const Vector3 ret( e.x_, e.y_, e.z_ );
    return ret;
}


void CelestialMotionRef::set_se3( const Ref<Se3Ref> & se3 )
{
    cm.set_se3( se3->se3 );
}


Ref<Se3Ref> CelestialMotionRef::get_se3() const
{
    const SE3 se3 = cm.get_se3();
    Ref<Se3Ref> se3_ref;
    se3_ref.instantiate();
    se3_ref->se3 = se3;
    return se3_ref;
}

real_t CelestialMotionRef::get_gm() const
{
    return cm.gm;
}





bool CelestialMotionRef::launch( real_t gm, const Ref<Se3Ref> & se3 )
{
    const bool ret = cm.launch( gm, se3->se3 );
	return ret;
}

real_t CelestialMotionRef::compute_gm_by_speed( real_t radius_km, real_t wanted_surface_orbit_velocity_kms ) const
{
    const Float gm = cm.compute_gm_by_speed( radius_km, wanted_surface_orbit_velocity_kms );
    return gm;
}

real_t CelestialMotionRef::compute_gm_by_period( real_t radius_km, real_t wanted_period_kms ) const
{
	const Float gm = cm.compute_gm_by_period( radius_km, wanted_period_kms );
	return gm;
}

void CelestialMotionRef::launch_elliptic( real_t gm, const Vector3 & unit_r, const Vector3 & unit_v, real_t period_hrs, real_t eccentricity )
{
    cm.launch_elliptic( gm, Vector3d(unit_r.x, unit_r.y, unit_r.z), Vector3d(unit_v.x, unit_v.y, unit_v.z), period_hrs, eccentricity );
}

Ref<Se3Ref> CelestialMotionRef::process( real_t dt )
{
    Ref<Se3Ref> se3;
    se3.instantiate();
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
    ret.instantiate();
    ret->cm = cm;

    return ret;
}

Dictionary CelestialMotionRef::serialize() const
{
    Dictionary data;
    data["type"]           = static_cast<int>( cm.type );
    data["allow_orbiting"] = cm.allow_orbiting;
    serialize_vector( cm.h, "h", data );
    serialize_vector( cm.e, "e", data );
    data["abs_e"]            = cm.abs_e;
    data["a"]                = cm.a;
    serialize_matrix( cm.A, "A", data );
    serialize_matrix( cm.inv_A, "inv_A", data );
    data["slr"]              = cm.slr;
    data["E"]                = cm.E;
    data["n"]                = cm.n;
    data["periapsis_t"]      = cm.periapsis_t;
    data["T"]                = cm.T;
    data["b"]                = cm.b;
    data["se3_local"]        = cm.se3_local.serialize();
    data["se3_global"]       = cm.se3_global.serialize();

    return data;
}

bool CelestialMotionRef::deserialize( const Dictionary & data )
{
	const int t = data["type"];
    cm.type           = static_cast<CelestialMotion::Type>( t );
    cm.allow_orbiting = data["allow_orbiting"];
    cm.h              = deserialize_vector( "h", data );
    cm.e              = deserialize_vector( "e", data );
    cm.abs_e          = data["abs_e"];
	cm.a              = data["a"];
    cm.A              = deserialize_matrix( "A", data );
    cm.inv_A          = deserialize_matrix( "inv_A", data );
    cm.slr            = data["slr"];
    cm.E              = data["E"];
    cm.n              = data["n"];
    cm.periapsis_t    = data["periapsis_t"];
    cm.T              = data["T"];
    cm.b              = data["b"];
    cm.se3_local.deserialize( data["se3_local"] );
    cm.se3_global.deserialize( data["se3_global"] );

    return true;
}

Array CelestialMotionRef::orbit_points( Node * orbiting_center, Node * camera_node, int qty, const Ref<ScaleDistanceRatioGd> & scale_distance_ratio, real_t base_scale )
{
	RefFrameNode * orbiting_center_node  = Node::cast_to<RefFrameNode>( orbiting_center );
	RefFrameNode * player_viewpoint_node = Node::cast_to<RefFrameNode>( camera_node );
	if ( (player_viewpoint_node == nullptr) || (orbiting_center_node == nullptr) )
		return Array();

	cm.orbit_points( orbiting_center_node, player_viewpoint_node, qty, pts );
	const int pts_qty = pts.size();
	Node3D * c = (camera_node != nullptr) ? Node::cast_to<Node3D>( camera_node ) : nullptr;

	// Retrieve scaler from the reference.
	const ScaleDistanceRatioGd * s = scale_distance_ratio.ptr();
	// If no scaler provided, return points as they are.
	if ( s == nullptr )
	{
		Array ret;
		ret.resize( pts_qty );
		for ( int i=0; i<pts_qty; i++ )
		{
			const Vector3d & r = pts.ptr()[i];
			ret.push_back( Vector3( r.x_, r.y_, r.z_ ) );
		}
		return ret;
	}

	// Apply distance shrink.
	for ( int i=0; i<pts_qty; i++ )
	{
		const Vector3d r = pts.ptr()[i];
		const Float scale = s->ratio.compute_scale( r, base_scale );
		const Vector3d scaled_r = r * (scale * base_scale);
		pts.ptrw()[i] = scaled_r;
	}

	Array ret;
	ret.resize( pts_qty );
	for ( int i=0; i<pts_qty; i++ )
	{
		const Vector3d r = pts.ptr()[i];
		const Vector3 at( r.x_, r.y_, r.z_ );
		ret.set( i, at );
	}
	return ret;
}

void CelestialMotionRef::set_force_numerical( bool en )
{
	cm.force_numerical = en;
}

bool CelestialMotionRef::get_force_numerical() const
{
	return cm.force_numerical;
}

void CelestialMotionRef::set_debug( bool en )
{
	cm._debug = en;
}

bool CelestialMotionRef::get_debug() const
{
	return cm._debug;
}



}




