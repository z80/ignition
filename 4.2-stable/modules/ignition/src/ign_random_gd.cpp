
#include "ign_random_gd.h"
#include <string>

namespace Ign
{


void IgnRandomGd::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("set_seed", "stri"), &IgnRandomGd::set_seed );
	ClassDB::bind_method( D_METHOD("get_seed"),         &IgnRandomGd::get_seed );

	ClassDB::bind_method( D_METHOD("integer"),          &IgnRandomGd::integer );
	ClassDB::bind_method( D_METHOD("integer_choices", "choices"), &IgnRandomGd::integer_choices );
	ClassDB::bind_method( D_METHOD("integer_interval_closed", "v_min", "v_max"), &IgnRandomGd::integer_interval_closed );

	ClassDB::bind_method( D_METHOD("floating_point_top_open"), &IgnRandomGd::floating_point_top_open );
	ClassDB::bind_method( D_METHOD("floating_point_closed"),   &IgnRandomGd::floating_point_closed );
	ClassDB::bind_method( D_METHOD("floating_point_open"),     &IgnRandomGd::floating_point_open );

	ClassDB::bind_method( D_METHOD("floating_point_limit_open", "limit"), &IgnRandomGd::floating_point_limit_open );
	ClassDB::bind_method( D_METHOD("floating_point_limit_closed", "v_max"), &IgnRandomGd::floating_point_limit_closed );
	ClassDB::bind_method( D_METHOD("floating_point_interval_closed", "v_min", "v_max"), &IgnRandomGd::floating_point_interval_closed );

	ClassDB::bind_method( D_METHOD("normal", "mean", "stddev"), &IgnRandomGd::normal );

	ClassDB::bind_method( D_METHOD("random_vector", "length"), &IgnRandomGd::random_vector );

	ClassDB::bind_method( D_METHOD("random_rotation", "axis", "angle_variation", "axis_variation"), &IgnRandomGd::random_rotation );

	ADD_PROPERTY( PropertyInfo( Variant::STRING, "seed" ), "set_seed", "get_seed" );
}

IgnRandomGd::IgnRandomGd()
{
}

IgnRandomGd::~IgnRandomGd()
{
}

void IgnRandomGd::set_seed( const String & stri )
{
	const CharString ch_stri = stri.ascii();
	const char * characters = ch_stri.get_data();
	const uint64_t s = std::stoull( characters );
	rand.seed( s );
}

String IgnRandomGd::get_seed() const
{
	const uint64_t s = rand.state();
	const String ret = uitos( s );
	return ret;
}

int IgnRandomGd::integer()
{
	const int ret = rand.integer();
	return ret;
}

int IgnRandomGd::integer_choices( int choices )
{
	const int ret = rand.integer_choices( choices );
	return ret;
}

int IgnRandomGd::integer_interval_closed( int v_min, int v_max )
{
	const int ret = rand.integer_interval_closed( v_min, v_max );
	return ret;
}


real_t IgnRandomGd::floating_point_top_open()
{
	const real_t ret = rand.floating_point_top_open();
	return ret;
}

real_t IgnRandomGd::floating_point_closed()
{
	const real_t ret = rand.floating_point_closed();
	return ret;
}

real_t IgnRandomGd::floating_point_open()
{
	const real_t ret = rand.floating_point_open();
	return ret;
}


real_t IgnRandomGd::floating_point_limit_open( real_t limit )
{
	const real_t ret = rand.floating_point_limit_open( limit );
	return ret;
}

real_t IgnRandomGd::floating_point_limit_closed( real_t v_max )
{
	const real_t ret = rand.floating_point_limit_closed( v_max );
	return ret;
}

real_t IgnRandomGd::floating_point_interval_closed( real_t v_min, real_t v_max )
{
	const real_t ret = rand.floating_point_interval_closed( v_min, v_max );
	return ret;
}


real_t IgnRandomGd::normal( Float mean, Float stddev )
{
	const real_t ret = rand.normal( mean, stddev );
	return ret;
}


Vector3 IgnRandomGd::random_vector( real_t length )
{
	const Vector3d v = rand.random_vector( length );
	const Vector3 ret( v.x_, v.y_, v.z_ );
	return ret;
}


Quaternion IgnRandomGd::random_rotation( const Vector3 & axis, real_t angle_variation, real_t axis_variation )
{
	const Quaterniond q = rand.random_rotation( Vector3d( axis.x, axis.y, axis.z ), angle_variation, axis_variation );
	const Quaternion ret( q.x_, q.y_, q.z_, q.w_ );
	return ret;
}


}

