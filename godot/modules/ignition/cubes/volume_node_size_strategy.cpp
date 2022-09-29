
#include "volume_node_size_strategy.h"
#include <iostream>

namespace Ign
{

VolumeNodeSizeStrategy::VolumeNodeSizeStrategy()
{
	radius      = 1000.0;
	focal_point = Vector3d( 0.0, 0.0, 0.0 );
}

VolumeNodeSizeStrategy::~VolumeNodeSizeStrategy()
{
}

void VolumeNodeSizeStrategy::set_radius( Float r )
{
	radius = r;
}

Float VolumeNodeSizeStrategy::get_radius() const
{
	return radius;
}

void VolumeNodeSizeStrategy::set_focal_point( const Vector3d & r )
{
	focal_point = r;
}

Vector3d VolumeNodeSizeStrategy::get_focal_point() const
{
	return focal_point;
}

void VolumeNodeSizeStrategy::set_height( Float h )
{
	height = h;
}

Float VolumeNodeSizeStrategy::get_height() const
{
	return height;
}

Float VolumeNodeSizeStrategy::local_node_size( const Vector3d & node_at, const Float node_size ) const
{
	const Float EPS   = 0.0001;
	const Vector3d a  = node_at - focal_point;
	const Float abs_a = a.Length();
	if (abs_a < EPS)
		return node_size;

	// Compute intersection point.
	const Float b = a.DotProduct( focal_point );
	const Float abs_node_at_2 = node_at.LengthSquared();
	const Float focal_point_2 = focal_point.LengthSquared();
	const Float abs_a_2 = abs_a * abs_a;
	const Float D_4 = b*b + abs_a_2*(radius*radius - focal_point_2);
	const Float t = ( std::sqrt( D_4 ) - b ) / abs_a_2;
	//std::cout << "t: " << t << std::endl;
	//if ( t > 1.0 )
	//	t *= 10.0;
	//const Float surface_dist = std::sqrt( std::sqrt(t) ) * abs_a;
	const Float surface_dist = t * abs_a;

	Float scale = surface_dist / (height * 1.41);
	if (scale < 1.0)
		scale = 1.0;

	const Float scaled_node_size = node_size / scale;

	return scaled_node_size;
}


}



