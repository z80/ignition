
#include "ref_frame_motion_node.h"
#include "scene/3d/spatial.h"
#include "core/engine.h"

#include "save_load.h"


namespace Ign
{

void RefFrameMotionNode::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("set_allow_orbiting", "en"), &RefFrameMotionNode::set_allow_orbiting );
	ClassDB::bind_method( D_METHOD("get_allow_orbiting"),       &RefFrameMotionNode::get_allow_orbiting, Variant::BOOL );

	ClassDB::bind_method( D_METHOD("stop"), &RefFrameMotionNode::stop );

	ClassDB::bind_method( D_METHOD("is_orbiting"),   &RefFrameMotionNode::is_orbiting, Variant::BOOL );
	ClassDB::bind_method( D_METHOD("movement_type"), &RefFrameMotionNode::movement_type, Variant::STRING );
	ClassDB::bind_method( D_METHOD("specific_angular_momentum"), &RefFrameMotionNode::specific_angular_momentum, Variant::REAL );
	ClassDB::bind_method( D_METHOD("eccentricity"), &RefFrameMotionNode::eccentricity, Variant::REAL );
	ClassDB::bind_method( D_METHOD("period"), &RefFrameMotionNode::period, Variant::REAL );
	ClassDB::bind_method( D_METHOD("time_after_periapsis"), &RefFrameMotionNode::time_after_periapsis, Variant::REAL );
	ClassDB::bind_method( D_METHOD("closest_approach"),     &RefFrameMotionNode::closest_approach, Variant::REAL );
	ClassDB::bind_method( D_METHOD("perigee"), &RefFrameMotionNode::perigee, Variant::REAL );
	ClassDB::bind_method( D_METHOD("apogee"),  &RefFrameMotionNode::apogee, Variant::REAL );
	ClassDB::bind_method( D_METHOD("min_velocity"), &RefFrameMotionNode::min_velocity, Variant::REAL );
	ClassDB::bind_method( D_METHOD("max_velocity"), &RefFrameMotionNode::max_velocity, Variant::REAL );
	ClassDB::bind_method( D_METHOD("excess_velocity"),  &RefFrameMotionNode::excess_velocity, Variant::REAL );
	ClassDB::bind_method( D_METHOD("deflection_angle"), &RefFrameMotionNode::deflection_angle, Variant::REAL );
	ClassDB::bind_method( D_METHOD("acceleration"),     &RefFrameMotionNode::acceleration, Variant::VECTOR3 );
	ClassDB::bind_method( D_METHOD("ex"), &RefFrameMotionNode::ex, Variant::VECTOR3 );
	ClassDB::bind_method( D_METHOD("ey"), &RefFrameMotionNode::ey, Variant::VECTOR3 );

	ClassDB::bind_method( D_METHOD("set_own_gm", "gm"), &RefFrameMotionNode::set_own_gm, Variant::REAL );
	ClassDB::bind_method( D_METHOD("get_own_gm"),       &RefFrameMotionNode::get_own_gm, Variant::REAL );

	ClassDB::bind_method( D_METHOD("get_gm"),        &RefFrameMotionNode::get_gm, Variant::REAL );

	ClassDB::bind_method( D_METHOD("launch"), &RefFrameMotionNode::launch, Variant::BOOL );
	ClassDB::bind_method( D_METHOD("compute_gm_by_speed",  "radius_km", "suface_orbit_velocity_kms"),    &RefFrameMotionNode::compute_gm_by_speed,  Variant::REAL );
	ClassDB::bind_method( D_METHOD("compute_gm_by_period", "radius_km", "period_kms"),                   &RefFrameMotionNode::compute_gm_by_period, Variant::REAL );
	ClassDB::bind_method( D_METHOD("launch_elliptic", "unit_r", "unit_v", "period_hrs", "eccentricity"), &RefFrameMotionNode::launch_elliptic,      Variant::BOOL );

	ClassDB::bind_method( D_METHOD("orbit_points",  "camera_node", "qty", "scale_distance_ratio", "base_scale"), &RefFrameMotionNode::orbit_points, Variant::POOL_VECTOR3_ARRAY );

	ClassDB::bind_method( D_METHOD("set_force_numerical", "en"), &RefFrameMotionNode::set_force_numerical );
	ClassDB::bind_method( D_METHOD("get_force_numerical"),       &RefFrameMotionNode::get_force_numerical, Variant::BOOL );

	ADD_GROUP( "Ignition", "" );
	ADD_PROPERTY( PropertyInfo( Variant::REAL,   "own_gm" ),          "set_own_gm",          "get_own_gm" );
	ADD_PROPERTY( PropertyInfo( Variant::BOOL,   "allow_orbiting" ),  "set_allow_orbiting",  "get_allow_orbiting" );
	//ADD_PROPERTY( PropertyInfo( Variant::OBJECT, "se3" ),             "set_se3",             "get_se3" );
	ADD_PROPERTY( PropertyInfo( Variant::BOOL,   "force_numerical" ), "set_force_numerical", "get_force_numerical" );
	ADD_PROPERTY( PropertyInfo( Variant::BOOL,   "debug" ),           "set_debug",           "get_debug" );
}

void RefFrameMotionNode::_notification( int p_notification )
{
	const bool is_editor = Engine::get_singleton()->is_editor_hint();
	if ( is_editor )
		return;

	switch (p_notification)
	{
		case NOTIFICATION_READY:
		{
			const StringName name = get_class_static();
			const bool in_group = is_in_group( name );
			if ( !in_group )
			{
				add_to_group( name );
			}
			//SceneTree * tree = get_tree();
			//tree->grou
		}
	}
}

RefFrameMotionNode::RefFrameMotionNode()
	: RefFrameNode()
{
}

RefFrameMotionNode::~RefFrameMotionNode()
{
}

void RefFrameMotionNode::set_se3_raw( const SE3 & se3 )
{
	RefFrameNode::set_se3_raw( se3 );
	const bool orbiting  = cm.is_orbiting();
	const bool allowed  = cm.get_allow_orbiting();
	if ( orbiting && allowed )
		launch();
}

void RefFrameMotionNode::set_allow_orbiting( bool en )
{
	const bool prev = cm.get_allow_orbiting();
	if ( prev && (!en) )
		print_line( "RefFrameMotionNode: reset allow_orbiting to false!!!" );
	cm.set_allow_orbiting( en );
}

bool RefFrameMotionNode::get_allow_orbiting() const
{
	const bool ret = cm.get_allow_orbiting();
	return ret;
}

void RefFrameMotionNode::stop()
{
	cm.stop();
}

bool RefFrameMotionNode::is_orbiting() const
{
	return cm.is_orbiting();
}

String RefFrameMotionNode::movement_type() const
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

real_t RefFrameMotionNode::specific_angular_momentum() const
{
	const real_t ret = cm.specific_angular_momentum();
	return ret;
}

real_t RefFrameMotionNode::eccentricity() const
{
	const real_t ret = cm.eccentricity();
	return ret;
}

real_t RefFrameMotionNode::period() const
{
	const real_t ret = cm.period();
	return ret;
}

real_t RefFrameMotionNode::time_after_periapsis() const
{
	const real_t ret = cm.time_after_periapsis();
	return ret;
}

real_t RefFrameMotionNode::closest_approach() const
{
	const real_t ret = cm.closest_approach();
	return ret;
}

real_t RefFrameMotionNode::perigee() const
{
	const real_t ret = cm.perigee();
	return ret;
}

real_t RefFrameMotionNode::apogee() const
{
	const real_t ret = cm.apogee();
	return ret;
}

real_t RefFrameMotionNode::min_velocity() const
{
	const real_t ret = cm.min_velocity();
	return ret;
}

real_t RefFrameMotionNode::max_velocity() const
{
	const real_t ret = cm.max_velocity();
	return ret;
}

real_t RefFrameMotionNode::excess_velocity() const
{
	const real_t ret = cm.excess_velocity();
	return ret;
}

real_t RefFrameMotionNode::deflection_angle() const
{
	const real_t ret = cm.deflection_angle();
	return ret;
}

Vector3 RefFrameMotionNode::acceleration() const
{
	const Vector3d acc_d = cm.acceleration();
	const Vector3 acc( acc_d.x_, acc_d.y_, acc_d.z_ );
	return acc;
}

Vector3 RefFrameMotionNode::ex() const
{
	const Vector3d e = cm.ex();
	const Vector3 ret( e.x_, e.y_, e.z_ );
	return ret;
}

Vector3 RefFrameMotionNode::ey() const
{
	const Vector3d e = cm.ey();
	const Vector3 ret( e.x_, e.y_, e.z_ );
	return ret;
}

void RefFrameMotionNode::set_own_gm( real_t gm )
{
	cm.own_gm = gm;
}

real_t RefFrameMotionNode::get_own_gm() const
{
	return cm.own_gm;
}

real_t RefFrameMotionNode::get_gm() const
{
	return cm.gm;
}

bool RefFrameMotionNode::launch()
{
	Node * p = get_parent();
	if ( p == nullptr )
		return false;
	RefFrameMotionNode * pm = Object::cast_to<RefFrameMotionNode>( p );
	if ( pm == nullptr )
		return false;

	const Float gm  = pm->cm.own_gm;
	const SE3   se3 = this->se3_;
	const bool  ret = cm.launch( gm, se3 );

	return ret;
}

real_t RefFrameMotionNode::compute_gm_by_speed( real_t radius_km, real_t wanted_surface_orbit_velocity_kms ) const
{
	const Float gm = cm.compute_gm_by_speed( radius_km, wanted_surface_orbit_velocity_kms );
	return gm;
}

real_t RefFrameMotionNode::compute_gm_by_period( real_t radius_km, real_t wanted_period_hrs ) const
{
	const Float gm = cm.compute_gm_by_period( radius_km, wanted_period_hrs );
	return gm;
}

bool RefFrameMotionNode::launch_elliptic( const Vector3 & unit_r, const Vector3 & unit_v, real_t period_hrs, real_t eccentricity )
{
	Node * p = get_parent();
	if ( p == nullptr )
		return false;
	RefFrameMotionNode * pm = Object::cast_to<RefFrameMotionNode>( p );
	if ( pm == nullptr )
		return false;

	const Float gm  = pm->cm.own_gm;
	cm.launch_elliptic( gm, Vector3d(unit_r.x, unit_r.y, unit_r.z), Vector3d(unit_v.x, unit_v.y, unit_v.z), period_hrs, eccentricity );

	return true;
}

Dictionary RefFrameMotionNode::serialize()
{
	Dictionary data = RefFrameNode::serialize();
	
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

bool RefFrameMotionNode::deserialize( const Dictionary & data )
{
	const bool ok = RefFrameNode::deserialize( data );
	if ( !ok )
		return false;

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

PoolVector3Array RefFrameMotionNode::orbit_points( Node * camera_node, int qty, const Ref<ScaleDistanceRatioGd> & scale_distance_ratio, real_t base_scale )
{
	RefFrameNode * orbiting_center_node  = this;
	RefFrameNode * player_viewpoint_node = Node::cast_to<RefFrameNode>( camera_node );
	if (player_viewpoint_node == nullptr)
		return PoolVector3Array();

	cm.orbit_points( orbiting_center_node, player_viewpoint_node, qty, pts );
	const int pts_qty = pts.size();
	Spatial * c = (camera_node != nullptr) ? Node::cast_to<Spatial>( camera_node ) : nullptr;

	// Retrieve scaler from the reference.
	const ScaleDistanceRatioGd * s = scale_distance_ratio.ptr();
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

	// Apply distance shrink.
	for ( int i=0; i<pts_qty; i++ )
	{
		const Vector3d r = pts.ptr()[i];
		const Float scale = s->ratio.compute_scale( r, base_scale );
		const Vector3d scaled_r = r * (scale * base_scale);
		pts.ptrw()[i] = scaled_r;
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

void RefFrameMotionNode::set_force_numerical( bool en )
{
	cm.force_numerical = en;
}

bool RefFrameMotionNode::get_force_numerical() const
{
	return cm.force_numerical;
}

void RefFrameMotionNode::set_debug( bool en )
{
	RefFrameNode::set_debug( en );
	cm._debug = en;
}

bool RefFrameMotionNode::get_debug() const
{
	return cm._debug;
}

void RefFrameMotionNode::_ign_physics_pre_process( real_t delta )
{
	if ( cm.is_orbiting() )
		this->se3_ = cm.process( delta );
	RefFrameNode::_ign_physics_pre_process( delta );
}

void RefFrameMotionNode::_ign_physics_process( real_t delta )
{
	RefFrameNode::_ign_physics_process( delta );
}

void RefFrameMotionNode::_ign_physics_post_process( real_t delta )
{
	RefFrameNode::_ign_physics_post_process( delta );
}


}

