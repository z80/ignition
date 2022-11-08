
#include "distance_scaler.h"
#include <cmath>


namespace Ign
{

DistanceScaler::DistanceScaler()
	: DistanceScalerBase()
{
    log_scale_  = 1.0;
}

DistanceScaler::~DistanceScaler()
{
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
    if ( dist <= plain_dist )
         return dist;

    const Float extra_dist = dist - plain_dist;
    const Float log_dist = std::log( 1.0 + extra_dist/log_scale_ ) * log_scale_;
    const Float scaled_dist = plain_dist + log_dist;

    return scaled_dist;
}

Vector3d DistanceScaler::scale( const Vector3d & v ) const
{
    const Float d = v.Length();
    const Float scaled_d = scale( d );
    if ( d > 0.0 )
    {
        const Float rel_d = scaled_d / d;
        const Vector3d scaled_v = rel_d * v;
        return scaled_v;
    }

    return v;
}

Vector3d DistanceScaler::unscale( const Vector3d & v ) const
{
    // scaled_r = plain_dist + log(1.0 + r - plain_dist) * log_scale;
    // log(1.0 + r - plain_dist) * scale = scaled_r - plain_dist;
    // r = plain_dist - 1.0 + exp( (scaled_r - plain_dist) / log_scale );

    const Float dist = v.Length();
    if (dist <= plain_dist)
        return v;
    
    const Float scaled_dist = plain_dist - 1.0 + std::exp( (dist - plain_dist) / log_scale_ );
    const Float scale       = scaled_dist / dist;
    const Vector3d scaled_v    = v * scale;

    return scaled_v;
}


}





