
#include "volume_node_size_strategy.h"
#include <iostream>

namespace Ign
{

VolumeNodeSizeStrategy::VolumeNodeSizeStrategy()
{
	radius        = 1000.0;
	min_distance  = 60.0;
	focal_point   = Vector3d( 0.0, 0.0, 0.0 );
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
	min_distance = h;
}

Float VolumeNodeSizeStrategy::get_height() const
{
	return min_distance;
}

void VolumeNodeSizeStrategy::clear_node_sizes()
{
	node_size_of_distance.clear();
}

void VolumeNodeSizeStrategy::append_node_size( Float distance, Float node_size )
{
	NodeSizeAtDistance d;
	d.distance  = distance;
	d.node_size = node_size;
	node_size_of_distance.push_back( d );
}

Float VolumeNodeSizeStrategy::local_node_size( const Vector3d & node_at, const Float node_size ) const
{
	const Float min_dist = min_distance;
	const Vector3d a  = node_at - focal_point;
	const Float abs_a = a.Length();
	if (abs_a <= min_dist)
		return node_size;

	//// Compute intersection point.
	//const Float b = a.DotProduct( focal_point );
	//const Float abs_node_at_2 = node_at.LengthSquared();
	//const Float focal_point_2 = focal_point.LengthSquared();
	//const Float abs_a_2 = abs_a * abs_a;
	//const Float D_4 = b*b + abs_a_2*(radius*radius - focal_point_2);
	//const Float t = ( std::sqrt( D_4 ) - b ) / abs_a_2;

	//// Let's compute distance along the sphere surface.
	//// Position on the surface.
	//const Vector3d node_surface_pt  = ( focal_point + a*t ) / radius;
	//const Vector3d focus_surface_pt = focal_point * ( 1.0 / focal_point.Length() );
	//const Float dot_product  = node_surface_pt.DotProduct( focus_surface_pt );
	//const Float angle        = std::acos( dot_product );
	//const Float geodesic_dist = angle * radius;
	//
	//// Distance from focal point to the projection
	//// point of the node center onto the sphere surface.
	//const Float surface_dist = t * abs_a;
	//const Float total_dist = geodesic_dist + surface_dist;
	//if ( total_dist < min_distance )
	//{
	//	return node_size;
	//}

	//const Float result_size  = node_size * min_distance / total_dist;

	const Float result_size = node_size * min_dist / abs_a;
	return result_size;
}

bool VolumeNodeSizeStrategy::can_subdivide( const Vector3d & node_at, const Float node_size, const Float min_node_size ) const
{
	const Float min_dist = min_distance;
	const Vector3d a  = node_at - focal_point;
	const Float abs_a = a.Length();
	if (abs_a <= min_dist)
	{
		const bool ret = (node_size > min_node_size);
		return ret;
	}

	// Min node size at current distance.
	const Float min_size_at_distance = min_node_size * abs_a / min_dist;
	bool can_subdivide_node = (node_size < min_size_at_distance);

	// Piecewise sizes.
	const int qty = node_size_of_distance.size();
	for ( int i=0; i<qty; i++ )
	{
		const NodeSizeAtDistance & size_dist = node_size_of_distance[i];
		const Float distance = size_dist.distance;
		if ( abs_a >= distance )
		{
			const Float local_node_size = size_dist.node_size;
			const bool should_not_subdivide = (node_size <= local_node_size);
			if ( should_not_subdivide )
			{
				can_subdivide_node = false;
				break;
			}
			else
			{
				can_subdivide_node = true;
			}
		}
	}

	return can_subdivide_node;
}



}



