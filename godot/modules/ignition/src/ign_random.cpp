
#include "ign_random.h"
#include <cmath>

namespace Ign
{

IgnRandom::IgnRandom()
	: PcgRandom()
{
}

IgnRandom::~IgnRandom()
{
}

IgnRandom::IgnRandom( const IgnRandom & inst )
{
	*this = inst;
}

const IgnRandom & IgnRandom::operator=( const IgnRandom & inst )
{
	if ( this != &inst )
	{
		PcgRandom::operator=( inst );
	}

	return *this;
}

int IgnRandom::integer()
{
	const int ret = static_cast<int>( uint( 0x7FFFFFFF ) );
	return ret;
}

int IgnRandom::integer_choices( int choices )
{
	const int ret = static_cast<int>( uint( choices ) );
	return ret;
}

int IgnRandom::integer_interval_closed( int v_min, int v_max )
{
	const int ret =  ( integer() % (1 + v_max - v_min) ) + v_min;
	return ret;
}

Float IgnRandom::floating_point_top_open()
{
	const Float ret = static_cast<Float>( integer() ) * ( 1.0 / static_cast<Float>(0x80000000) ); // divided by 2^32
	return ret;
}

Float IgnRandom::floating_point_closed()
{
	const Float ret = static_cast<Float>( integer() ) * (1.0 / static_cast<Float>(0x7FFFFFFF)); // divided by 2^32 - 1
	return ret;
}

Float IgnRandom::floating_point_open()
{
	const Float ret = ( static_cast<Float>(integer()) + .5) * (1. / static_cast<Float>(0x80000000)); // divided by 2^32
	return ret;
}

Float IgnRandom::floating_point_limit_open( Float limit )
{
	const Float ret = limit * floating_point_top_open();
	return ret;
}

Float IgnRandom::floating_point_limit_closed( Float v_max )
{
	const Float ret = v_max * floating_point_closed();
	return ret;
}

Float IgnRandom::floating_point_interval_closed( Float v_min, Float v_max )
{
	const Float ret = floating_point_limit_closed(v_max - v_min) + v_min;
	return ret;
}

Float IgnRandom::normal( Float mean, Float stddev )
{
	// https://en.wikipedia.org/wiki/Box-Muller_transform#Polar_form
	Float u, v, s, z0;

	do {
		u = floating_point_interval_closed( -1.0, 1.0 );
		v = floating_point_interval_closed( -1.0, 1.0 );
		s = u * u + v * v;
	} while (s >= 1.0);

	s = std::sqrt( ( -2.0 * std::log(s) ) / s );
	z0 = u * s;

	const Float ret = mean + z0 * stddev;

	return ret;
}

Vector3d IgnRandom::random_vector( Float length )
{
	Vector3d v( floating_point_interval_closed(-1.0, 1.0),
		        floating_point_interval_closed(-1.0, 1.0),
		        floating_point_interval_closed(-1.0, 1.0) );
	v.Normalize();
	v = v * length;
	return v;
}

Quaterniond IgnRandom::random_rotation( const Vector3d & axis, Float angle_variation, Float axis_variation )
{
	const Float angle = floating_point_interval_closed( -angle_variation, angle_variation );
	Vector3d    axis_final = axis;
	if ( axis_variation > 0.0 )
	{
		axis_final += Vector3d( floating_point_interval_closed(-axis_variation, axis_variation),
			                    floating_point_interval_closed(-axis_variation, axis_variation),
			                    floating_point_interval_closed(-axis_variation, axis_variation) );
	}
	axis_final.Normalize();
	Quaterniond q( angle*M_RADTODEG, axis_final );

	return q;
}





}

