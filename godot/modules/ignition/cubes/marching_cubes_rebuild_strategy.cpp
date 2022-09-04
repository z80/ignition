
#include "marching_cubes_rebuild_strategy.h"


namespace Ign
{

MarchingCubesRebuildStrategy::MarchingCubesRebuildStrategy()
{
	focus         = Vector3d( 0.0, 0.0, 0.0 );
	height        = 100.0;
	planet_radius = 1000.0;
	rescale_dist  = 30.0;
	rebuild_dist  = 50.0;
}

MarchingCubesRebuildStrategy::~MarchingCubesRebuildStrategy()
{
}


void MarchingCubesRebuildStrategy::init( Float planet_radius, Float height, Float rescale_dist, Float rebuild_dist )
{
	this->planet_radius = planet_radius;
	this->height        = height;
	this->rescale_dist  = rescale_dist;
	this->rebuild_dist  = rebuild_dist;
}


bool MarchingCubesRebuildStrategy::need_rebuild( const SE3 & view_point_se3 )
{
    const Vector3d camera_r = view_point_se3.r();
	Float camera_dist = camera_r.Length();
	camera_dist = (camera_dist < planet_radius) ? camera_dist : planet_radius;
	const Float d_dist      = camera_dist - planet_radius;
	const Float d_dist_2    = d_dist*d_dist;
	const Float focal_dist = (planet_radius - height) / ( 1.0 + d_dist_2 );

	const Vector3d focal_point = camera_r * (focal_dist / camera_dist);
	const Vector3d d_focal_point = focal_point - focus;
	const Float    d_focus = d_focal_point.Length();

	const bool ret = (d_focus >= rebuild_dist);

	if ( ret )
	{
		focus = focal_point;
	}

    return ret;
}



bool MarchingCubesRebuildStrategy::need_rescale( const SE3 & view_point_se3 )
{
	const Vector3d camera_r = view_point_se3.r();
	Float camera_dist = camera_r.Length();
	camera_dist = (camera_dist < planet_radius) ? camera_dist : planet_radius;
	const Float d_dist      = camera_dist - planet_radius;
	const Float d_dist_2    = d_dist*d_dist;
	const Float focal_dist = (planet_radius - height) / ( 1.0 + d_dist_2 );

	const Vector3d focal_point = camera_r * (focal_dist / camera_dist);
	const Vector3d d_focal_point = focal_point - focus;
	const Float    d_focus = d_focal_point.Length();

	const bool ret = (d_focus >= rescale_dist);

	if ( ret )
	{
		focus = focal_point;
	}

	return ret;
}

Float MarchingCubesRebuildStrategy::local_node_size( const Vector3d & node_at, const Float node_size ) const
{
	const Float EPS   = 0.0001;
	const Vector3d a  = node_at - focus;
	const Float abs_a = a.Length();
	if (abs_a < EPS)
		return node_size;

	// Compute intersection point.
	const Float b = a.DotProduct( focus );
	const Float abs_node_at_2 = node_at.LengthSquared();
	const Float focal_point_2 = focus.LengthSquared();
	const Float abs_a_2 = abs_a * abs_a;
	const Float D_4 = b*b + abs_a_2*(planet_radius*planet_radius - focal_point_2);
	const Float t = ( std::sqrt( D_4 ) - b*0.5 ) / abs_a_2;
	const Float surface_dist = t * abs_a;

	Float scale = surface_dist / (height * 1.41);
	if (scale < 1.0)
		scale = 1.0;

	const Float scaled_node_size = node_size / scale;

	return scaled_node_size;
}






}












