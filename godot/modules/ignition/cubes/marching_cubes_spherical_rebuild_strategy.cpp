
#include "marching_cubes_spherical_rebuild_strategy.h"
#include "distance_scaler_base.h"

namespace Ign
{

MarchingCubesSphericalRebuildStrategy::MarchingCubesSphericalRebuildStrategy()
    : MarchingCubesRebuildStrategy()
{
    rebuild_angle = 0.02;
}

MarchingCubesSphericalRebuildStrategy::~MarchingCubesSphericalRebuildStrategy()
{
}

void MarchingCubesSphericalRebuildStrategy::initialize( Float radius, const DistanceScalerBase * scaler )
{
    const Float plain_dist = scaler->plain_distance() * 0.3;
    rebuild_angle = plain_dist / radius;
}

void MarchingCubesSphericalRebuildStrategy::set_rebuild_angle( Float ang )
{
	rebuild_angle = ang;
}

Float MarchingCubesSphericalRebuildStrategy::get_rebuild_angle() const
{
	return rebuild_angle;
}

bool MarchingCubesSphericalRebuildStrategy::need_rebuild( const SE3 & view_point_se3 )
{
    if ( !initialized )
    {
        initialized = true;
        last_se3    = view_point_se3;

        return true;
    }

    const Float v_dot   = last_se3.r_.DotProduct( view_point_se3.r_ );
    const Float v_cross = last_se3.r_.CrossProduct( view_point_se3.r_ ).Length();

    const Float angle = std::atan2( v_cross, v_dot );

    if ( angle >= rebuild_angle )
    {
        last_se3 = view_point_se3;
        return true;
    }

    return false;
}





}








