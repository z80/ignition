
#include "marching_cubes_rebuild_strategy.h"


namespace Ign
{

MarchingCubesRebuildStrategy::MarchingCubesRebuildStrategy()
{
	focal_point_rebuild = Vector3d( 0.0, 0.0, 0.0 );
	focal_point_rescale = Vector3d( 0.0, 0.0, 0.0 );
	height        = 100.0;
	planet_radius = 1000.0;
	rescale_dist  = 30.0;
	rebuild_dist  = 50.0;
}

MarchingCubesRebuildStrategy::~MarchingCubesRebuildStrategy()
{
}


bool MarchingCubesRebuildStrategy::need_rebuild( const SE3 & view_point_se3 )
{
    Vector3d camera_r = view_point_se3.r();
	Float camera_dist = camera_r.Length();
	if (camera_dist < planet_radius)
	{
		camera_r    = camera_r / camera_dist * planet_radius;
		camera_dist = planet_radius;
	}

	const Float d_dist_rel      = (camera_dist - planet_radius)/planet_radius;
	const Float d_dist_rel_2    = d_dist_rel*d_dist_rel;
	const Float focal_dist = (planet_radius - height) / ( 1.0 + d_dist_rel_2 );

	const Vector3d focal_point = camera_r * (focal_dist / camera_dist);
	const Vector3d d_focal_point = focal_point - focal_point_rebuild;
	const Float    d_focus = d_focal_point.Length();

	const Float scaled_rebuild_dist = rebuild_dist * std::sqrt( planet_radius / camera_dist );

	const bool ret = (d_focus >= scaled_rebuild_dist);

	if ( ret )
	{
		focal_point_rebuild = focal_point;
		focal_point_rescale = focal_point;
	}

    return ret;
}



bool MarchingCubesRebuildStrategy::need_rescale( const SE3 & view_point_se3 )
{
	Vector3d camera_r = view_point_se3.r();
	Float camera_dist = camera_r.Length();
	if (camera_dist < planet_radius)
	{
		camera_r    = camera_r / camera_dist * planet_radius;
		camera_dist = planet_radius;
	}

	const Float d_dist_rel      = (camera_dist - planet_radius)/planet_radius;
	const Float d_dist_rel_2    = d_dist_rel*d_dist_rel;
	const Float focal_dist = (planet_radius - height) / ( 1.0 + d_dist_rel_2 );

	const Vector3d focal_point = camera_r * (focal_dist / camera_dist);
	const Vector3d d_focal_point = focal_point - focal_point_rescale;
	const Float    d_focus = d_focal_point.Length();

	const Float scaled_rescale_dist = rescale_dist * std::sqrt( planet_radius / camera_dist );

	const bool ret = (d_focus >= scaled_rescale_dist);

	if ( ret )
	{
		focal_point_rescale = focal_point;
	}

	return ret;
}

Vector3d MarchingCubesRebuildStrategy::get_focal_point_rebuild() const
{
	return focal_point_rebuild;
}

Vector3d MarchingCubesRebuildStrategy::get_focal_point_rescale() const
{
	return focal_point_rescale;
}

void MarchingCubesRebuildStrategy::set_radius( Float r )
{
	planet_radius = r;
}

Float MarchingCubesRebuildStrategy::get_radius() const
{
	return planet_radius;
}

void MarchingCubesRebuildStrategy::set_height( Float h )
{
	height = h;
}

Float MarchingCubesRebuildStrategy::get_height() const
{
	return height;
}

void MarchingCubesRebuildStrategy::set_rescale_dist( Float dist )
{
	rescale_dist = dist;
}

Float MarchingCubesRebuildStrategy::get_rescale_dist() const
{
	return rescale_dist;
}

void MarchingCubesRebuildStrategy::set_rebuild_dist( Float dist )
{
	rebuild_dist = dist;
}

Float MarchingCubesRebuildStrategy::get_rebuild_dist() const
{
	return rebuild_dist;
}










}












