
#include "distance_scaler.h"
#include <cmath>


namespace Ign
{

DistanceScaler::DistanceScaler()
{
	plain_dist_ = 100.0;
	log_scale_  = 1.0;
}

DistanceScaler::~DistanceScaler()
{
}

void DistanceScaler::set_plain_distance( Float dist )
{
	plain_dist_ = dist;
}

Float DistanceScaler::plain_distance() const
{
	return plain_dist_;
}

void DistanceScaler::set_log_scale( Float scale )
{
	log_scale_ = scale;
}


Float DistanceScaler::log_scale() const
{
	return log_scale_;
}

Float DistanceScaler::scale( Float dist ) const
{
	if ( dist <= plain_dist_ )
		return dist;

	const Float extra_dist = dist - plain_dist_;
	const Float log_dist = std::log( 1.0 + extra_dist ) * log_scale_;
	const Float scaled_dist = plain_dist_ + log_dist;

	return scaled_dist;
}

}





