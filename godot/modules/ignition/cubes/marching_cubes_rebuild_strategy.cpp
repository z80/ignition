
#include "marching_cubes_rebuild_strategy.h"


namespace Ign
{

MarchingCubesRebuildStrategy::MarchingCubesRebuildStrategy()
{
	focal_point_rebuild = Vector3d( 0.0, 0.0, 0.0 );
	focal_point_rescale = Vector3d( 0.0, 0.0, 0.0 );
	height        = 100.0;
	planet_radius = 1000.0;

	rebuild_dist  = 50.0;

	rescale_close_dist   = 5.0;
	rescale_far_tangent = 10.0 / 180.0 * 3.14;
	rescale_rel_tangent = 1.0 / 180.0 * 3.14;
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

	const Float d_dist_rel   = camera_dist/planet_radius;
	const Float d_dist_rel_2 = d_dist_rel*d_dist_rel;
	const Float focal_dist   = 2.0*(planet_radius - height) / ( 1.0 + d_dist_rel_2 );

	const Vector3d focal_point   = camera_r * (focal_dist / camera_dist);
	const Vector3d d_focal_point = focal_point - focal_point_rebuild;
	const Float    d_focus       = d_focal_point.Length();

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

	const Float d_dist_rel   = camera_dist/planet_radius;
	const Float d_dist_rel_2 = d_dist_rel*d_dist_rel;
	const Float focal_dist   = 2.0*(planet_radius - height) / ( 1.0 + d_dist_rel_2 );

	const Vector3d focal_point = camera_r * (focal_dist / camera_dist);
	const Vector3d d_focal_point = focal_point - focal_point_rescale;
	if ( d_focal_point.Length() > 0.0 )
	{
		int i=0;
	}

	const Vector3d d_focal_point_norm = camera_r * ( camera_r.DotProduct( d_focal_point ) / (camera_dist*camera_dist) );
	const Vector3d d_focal_point_tang = d_focal_point - d_focal_point_norm;


	const Float r     = camera_dist;
	const Float R     = planet_radius;
	const Float h     = height;
	const Float R_r   = R / r;
	const Float R_r_2 = R_r * R_r;

	// Tangential distance.
	{
		//D: ( 2*k*(R-h) - ( 2*k*(R-h) - d ) * (R/r) ) * (R/r)^2
		const Float d = rescale_close_dist;
		const Float k = rescale_far_tangent;

		const Float _2_k_R_h = 2.0*k*(R-h);
		const Float D = ( _2_k_R_h - (_2_k_R_h - d) * R_r ) * R_r_2;

		const Float abs_d_focal_point_tang = d_focal_point_tang.Length();
		if ( abs_d_focal_point_tang >= D )
		{
			focal_point_rescale = focal_point;
			return true;
		}
	}

	// Normal distance.
	{
		const Float _4_R_h = 4.0*(R-h);
		const Float _1_R_r_2 = 1.0 + R_r_2;
		const Float _1_R_r_2_2 = _1_R_r_2 * _1_R_r_2;
		const Float den = _1_R_r_2_2 / R_r_2;
		const Float k = rescale_rel_tangent;
		const Float D = (_4_R_h * k) / den;

		const Float abs_d_focal_dist_norm = d_focal_point_norm.Length();

		if ( abs_d_focal_dist_norm >= D )
		{
			focal_point_rescale = focal_point;
			return true;
		}
	}

	return false;
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

void MarchingCubesRebuildStrategy::set_rebuild_dist( Float dist )
{
	rebuild_dist = dist;
}

Float MarchingCubesRebuildStrategy::get_rebuild_dist() const
{
	return rebuild_dist;
}

void MarchingCubesRebuildStrategy::set_rescale_close_dist( Float dist )
{
	rescale_close_dist = dist;
}

Float MarchingCubesRebuildStrategy::get_rescale_close_dist() const
{
	return rescale_close_dist;
}

void MarchingCubesRebuildStrategy::set_rescale_far_tangent( Float dist )
{
	rescale_far_tangent = dist;
}

Float MarchingCubesRebuildStrategy::get_rescale_far_tangent() const
{
	return rescale_far_tangent;
}

void MarchingCubesRebuildStrategy::set_rescale_depth_rel_tangent( Float dist )
{
	rescale_rel_tangent = dist;
}

Float MarchingCubesRebuildStrategy::get_rescale_depth_rel_tangent() const
{
	return rescale_rel_tangent;
}










}












