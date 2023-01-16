
#include "scale_distance_ratio.h"

namespace Ign
{

ScaleDistanceRatio::ScaleDistanceRatio()
{
	max_distance = 1000.0;
}

ScaleDistanceRatio::~ScaleDistanceRatio()
{
}

void ScaleDistanceRatio::set_max_distance( Float dist )
{
	max_distance = dist;
}

Float ScaleDistanceRatio::get_max_distance() const
{
	return max_distance;
}

SE3 ScaleDistanceRatio::compute_transform( const SE3 & rel_se3, Float base_scale, Float & scale ) const
{
	const Float dist = rel_se3.r_.Length();
	const Float denominator = max_distance + base_scale * dist;

	// Return scale separately.
	scale = max_distance / denominator;

	const Float scaled_dist_over_dist = (base_scale * max_distance) / denominator;

	// Returned transform without scale.
	SE3 ret_se3( rel_se3 );
	ret_se3.r_ *= scaled_dist_over_dist;

	return ret_se3;
}


}

