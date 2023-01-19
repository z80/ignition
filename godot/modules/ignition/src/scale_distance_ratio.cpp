
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

Float ScaleDistanceRatio::compute_scale( const SE3 & rel_se3, Float base_scale ) const
{
	const Float dist = rel_se3.r_.Length();
	const Float denominator = max_distance + base_scale * dist;

	// Return scale separately.
	const Float scale = max_distance / denominator;

	return scale;
}


}

