
#include "celestial_rotation.h"
#include <cmath>

namespace Ign
{

CelestialRotation::CelestialRotation()
{
	period = 1000;
	time   = 0;
}

CelestialRotation::~CelestialRotation()
{
}

void CelestialRotation::init( const Vector3d & up, Float period_hrs )
{
	axis_orientation = Quaterniond( Vector3d( 0.0, 1.0, 0.0 ), up );
	time = 0;
	const Float T = period_hrs * 3600.0;
	period = Celestial::secs_to_ticks( T );
	const Float w = PI2 / T;
	se3.q_ = axis_orientation;
	se3.w_ = axis_orientation * Vector3d( 0.0, w, 0.0 );
}

const SE3 & CelestialRotation::process( Float dt )
{
	const Celestial::Ticks d_time = Celestial::secs_to_ticks( dt );
	time += d_time;
	if ( time >= period )
		time -= period;
	const Float angle_2 = ( PI1 * static_cast<Float>(time) ) / static_cast<Float>(period);

	const Float co2 = std::cos( angle_2 );
	const Float si2 = std::sin( angle_2 );

	const Quaterniond q( co2, 0.0, si2, 0.0 );
	se3.q_ = axis_orientation * q;

	return se3;
}






}



