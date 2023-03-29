
#include "volume_node_size_strategy.h"
#include <iostream>

namespace Ign
{

VolumeNodeSizeStrategy::VolumeNodeSizeStrategy()
{
	radius        = 1000.0;
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

void VolumeNodeSizeStrategy::set_max_level( int level )
{
	max_level = level;
}

int VolumeNodeSizeStrategy::get_max_level() const
{
	return max_level;
}

int VolumeNodeSizeStrategy::compute_max_level( Float min_detail_size )
{
	// min_detail_size * 2 ^ level = 2*radius;
	// level = log_base_2(2*radius/min_detail_size);
	// level = log(2*radius/min_detail_size) / log(2);
	Float lvl = std::log( 2.0*radius/min_detail_size );
	max_level = static_cast<int>( std::ceil( lvl ) );
	return max_level;
}


Vector3d VolumeNodeSizeStrategy::warp( const Vector3d & r ) const
{
	const Float abs_r = r.Length();
	if ( abs_r < 0.001 )
		return r;
	const Float abs_r2   = abs_r*abs_r;
	const Float abs_f2   = focal_point.LengthSquared();
	const Float dot_r_f  = r.DotProduct( focal_point );
	const Float dot_r_f2 = dot_r_f * dot_r_f;
	const Float R2       = radius*radius;

	const Float numerator   = std::sqrt( dot_r_f2 + (R2 - abs_f2)*abs_r2 ) - dot_r_f;
	const Float denominator = radius * abs_r;
	const Float k           = numerator / denominator;

	const Vector3d scaled_r = r * k;
	const Vector3d ret      = focal_point + scaled_r;

	return ret;
}



}



