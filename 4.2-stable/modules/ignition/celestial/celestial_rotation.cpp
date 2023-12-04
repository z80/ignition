
#include "celestial_rotation.h"
#include "save_load.h"
#include <cmath>

namespace Ign
{

CelestialRotation::CelestialRotation()
{
    period = 1000;
    time   = 0;
    spinning = false;
}

CelestialRotation::~CelestialRotation()
{
}

void CelestialRotation::init( const Vector3d & up, Float period_hrs )
{
    spinning = true;

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
    if ( !spinning )
        return se3;

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

Dictionary CelestialRotation::serialize()
{
	Dictionary d;

	d["spinning"] = spinning;
	d["period"]   = period;
	d["time"]     = time;

	Dictionary q;
	serialize_quat( axis_orientation, "axis_orientation", q );
	d["axis_orientation"] = q;

	d["se3"] = se3.serialize();

	return d;
}

bool CelestialRotation::deserialize( const Dictionary & data )
{
	spinning = data["spinning"];
	period   = data["period"];
	time     = data["time"];
	axis_orientation = deserialize_quat( "axis_orientation", data );

	{
		const Dictionary se3_data = data["se3"];
		const bool ok = se3.deserialize( se3_data );
		if ( !ok )
			return false;
	}

	return true;
}







}



