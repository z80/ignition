
#include "volume_node_size_strategy.h"
#include <iostream>

namespace Ign
{

VolumeNodeSizeStrategy::VolumeNodeSizeStrategy()
{
	radius        = 1000.0;
	focal_point   = Vector3d( 0.0, 0.0, 0.0 );

	max_distance  = 500.0;
	max_node_size = 100.0;
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

void VolumeNodeSizeStrategy::set_max_distance( Float dist )
{
	max_distance = dist;
}

Float VolumeNodeSizeStrategy::get_max_distance() const
{
	return max_distance;
}

void VolumeNodeSizeStrategy::set_max_node_size( Float sz )
{
	max_node_size = sz;
}

Float VolumeNodeSizeStrategy::get_max_node_size() const
{
	return max_node_size;
}

Float VolumeNodeSizeStrategy::local_node_size( const Vector3d & node_at, const Float node_size ) const
{
	const Float min_distance = height * 1.41;
	const Vector3d a  = node_at - focal_point;
	const Float abs_a = a.Length();
	if (abs_a <= min_distance)
		return node_size;

	// Compute intersection point.
	const Float b = a.DotProduct( focal_point );
	const Float abs_node_at_2 = node_at.LengthSquared();
	const Float focal_point_2 = focal_point.LengthSquared();
	const Float abs_a_2 = abs_a * abs_a;
	const Float D_4 = b*b + abs_a_2*(radius*radius - focal_point_2);
	const Float t = ( std::sqrt( D_4 ) - b ) / abs_a_2;
	
	// Distance from focal point to the projection
	// point of the node center onto the sphere surface.
	const Float surface_dist = t * abs_a;
	if ( surface_dist < min_distance )
	{
		return node_size;
	}
	else if ( surface_dist > max_distance )
	{
		const Float scale = node_size / max_node_size;
		const Float scaled_node_size = node_size * scale;
		return scaled_node_size;
	}

	const Float max_scale         = node_size / max_node_size;
	const Float scale             = 1.0 + (max_scale - 1.0) * (surface_dist - min_distance) / (max_distance - min_distance);
	const Float scaled_node_size  = node_size * scale;
	return scaled_node_size;
}


}



