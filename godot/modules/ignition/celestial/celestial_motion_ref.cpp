
#include "celestial_motion_ref.h"
#include "ref_frame_node.h"
#include "save_load.h"

namespace Ign
{

void CelestialMotionRef::_bind_methods()
{
    ClassDB::bind_method( D_METHOD("set_allow_orbiting", "en"), &CelestialMotionRef::set_allow_orbiting );
    ClassDB::bind_method( D_METHOD("get_allow_orbiting"), &CelestialMotionRef::get_allow_orbiting, Variant::BOOL );

    ClassDB::bind_method( D_METHOD("stop"), &CelestialMotionRef::stop );

    ClassDB::bind_method( D_METHOD("is_orbiting"), &CelestialMotionRef::is_orbiting, Variant::BOOL );
    ClassDB::bind_method( D_METHOD("movement_type"), &CelestialMotionRef::movement_type, Variant::STRING );
    ClassDB::bind_method( D_METHOD("specific_angular_momentum"), &CelestialMotionRef::specific_angular_momentum, Variant::REAL );
    ClassDB::bind_method( D_METHOD("eccentricity"), &CelestialMotionRef::eccentricity, Variant::REAL );
    ClassDB::bind_method( D_METHOD("period"), &CelestialMotionRef::period, Variant::REAL );
    ClassDB::bind_method( D_METHOD("time_after_periapsis"), &CelestialMotionRef::time_after_periapsis, Variant::REAL );
    ClassDB::bind_method( D_METHOD("closest_approach"), &CelestialMotionRef::closest_approach, Variant::REAL );
    ClassDB::bind_method( D_METHOD("perigee"), &CelestialMotionRef::perigee, Variant::REAL );
    ClassDB::bind_method( D_METHOD("apogee"), &CelestialMotionRef::apogee, Variant::REAL );
    ClassDB::bind_method( D_METHOD("min_velocity"), &CelestialMotionRef::min_velocity, Variant::REAL );
    ClassDB::bind_method( D_METHOD("max_velocity"), &CelestialMotionRef::max_velocity, Variant::REAL );
    ClassDB::bind_method( D_METHOD("excess_velocity"), &CelestialMotionRef::excess_velocity, Variant::REAL );
    ClassDB::bind_method( D_METHOD("deflection_angle"), &CelestialMotionRef::deflection_angle, Variant::REAL );
    ClassDB::bind_method( D_METHOD("ex"), &CelestialMotionRef::ex, Variant::VECTOR3 );
    ClassDB::bind_method( D_METHOD("ey"), &CelestialMotionRef::ey, Variant::VECTOR3 );

    ClassDB::bind_method( D_METHOD("set_se3", "se3"), &CelestialMotionRef::set_se3 );
    ClassDB::bind_method( D_METHOD("get_se3"),        &CelestialMotionRef::get_se3, Variant::OBJECT );

    ClassDB::bind_method( D_METHOD("get_gm"),        &CelestialMotionRef::get_gm, Variant::REAL );

    ClassDB::bind_method( D_METHOD("init", "gm", "se3"), &CelestialMotionRef::init );
    ClassDB::bind_method( D_METHOD("init_gm", "radius_km", "suface_orbit_velocity_kms"), &CelestialMotionRef::init_gm );
    ClassDB::bind_method( D_METHOD("launch_elliptic", "gm", "unit_r", "unit_v", "period_hrs", "eccentricity"), &CelestialMotionRef::launch_elliptic );
    ClassDB::bind_method( D_METHOD("process", "dt"), &CelestialMotionRef::process, Variant::OBJECT );
    ClassDB::bind_method( D_METHOD("process_rf", "dt", "rf"), &CelestialMotionRef::process_rf );
    ClassDB::bind_method( D_METHOD("duplicate"), &CelestialMotionRef::duplicate, Variant::OBJECT );

	ClassDB::bind_method( D_METHOD("orbit_points", "orbiting_center_node", "player_viewpoint_node", "camera_node", "scaler", "qty"), &CelestialMotionRef::orbit_points, Variant::POOL_VECTOR3_ARRAY );

	ClassDB::bind_method( D_METHOD("set_force_numerical", "en"), &CelestialMotionRef::set_force_numerical );
	ClassDB::bind_method( D_METHOD("get_force_numerical"),       &CelestialMotionRef::get_force_numerical, Variant::BOOL );

	ClassDB::bind_method( D_METHOD("set_debug", "en"), &CelestialMotionRef::set_debug );
	ClassDB::bind_method( D_METHOD("get_debug"), &CelestialMotionRef::get_debug, Variant::BOOL );

	ADD_GROUP( "Ignition", "" );
    ADD_PROPERTY( PropertyInfo( Variant::BOOL,   "allow_orbiting" ),       "set_allow_orbiting",       "get_allow_orbiting" );
    ADD_PROPERTY( PropertyInfo( Variant::OBJECT, "se3" ),                  "set_se3",                  "get_se3" );
	ADD_PROPERTY( PropertyInfo( Variant::BOOL,   "force_numerical" ),      "set_force_numerical",      "get_force_numerical" );
	ADD_PROPERTY( PropertyInfo( Variant::BOOL,   "debug" ),                "set_debug",                "get_debug" );
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
    se3_ref.instance();
    se3_ref->se3 = se3;
    return se3_ref;
}

real_t CelestialMotionRef::get_gm() const
{
    return cm.gm;
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

PoolVector3Array CelestialMotionRef::orbit_points( Node * orbiting_center, Node * player_viewpoint, Node * camera_node, Ref<DistanceScalerRef> scaler, int qty )
{
	RefFrameNode * orbiting_center_node  = Node::cast_to<RefFrameNode>( orbiting_center );
	RefFrameNode * player_viewpoint_node = Node::cast_to<RefFrameNode>( player_viewpoint );
	if (player_viewpoint_node == nullptr)
		return PoolVector3Array();

	cm.orbit_points( orbiting_center_node, player_viewpoint_node, qty, pts );
	const int pts_qty = pts.size();
	Spatial * c = (camera_node != nullptr) ? Node::cast_to<Spatial>( camera_node ) : nullptr;

	// Retrieve scaler from the reference.
	DistanceScalerRef * s = scaler.ptr();
	// If no scaler provided, return points as they are.
	if ( s == nullptr )
	{
		PoolVector3Array ret;
		ret.resize( pts_qty );
		for ( int i=0; i<pts_qty; i++ )
		{
			const Vector3d & r = pts.ptr()[i];
			ret.push_back( Vector3( r.x_, r.y_, r.z_ ) );
		}
		return ret;
	}

	// Get camera transform if there is a place.
	SE3 camera_se3;
	Transform camera_t;
	if (c != nullptr)
		camera_t = c->get_global_transform();
	else
		camera_t = Transform( 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0 );
	const Vector3 r = camera_t.origin;
	camera_se3.r_ = Vector3d( r.x, r.y, r.z );
	const Quat q = camera_t.basis.get_rotation_quat();
	camera_se3.q_ = q;

	// Inverted camera transform.
	const SE3 inv_camera_se3 = camera_se3.inverse();

	// Apply scaler relative to the camera.
	// Convert to camera ref. frame, apply scaler and convert back.
	for ( int i=0; i<pts_qty; i++ )
	{
		const Vector3d r = pts.ptr()[i];
		const Vector3d rel_r = inv_camera_se3.r_ + (inv_camera_se3.q_ * r);
		const Float d = rel_r.Length();
		if (d < Celestial::EPS)
			continue;
		
		const Float scale = s->scale( d );
		const Vector3d scaled_rel_r = rel_r * (scale / d);
		// Convert back to player ref. frame.
		const Vector3d abs_r = camera_se3.r_ + (camera_se3.q_ * scaled_rel_r);
		pts.ptrw()[i] = abs_r;
	}

	PoolVector3Array ret;
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




