
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
    // scaled_r = plain_dist + log(r - plain_dist) * log_scale;
    // log(r - plain_dist) * scale = scaled_r - plain_dist;
    // r = plain_dist + exp( (scaled_r - plain_dist) / log_scale );

    const Float dist = v.Length();
    if (dist <= plain_dist_)
        return v;
    
    const Float scaled_dist = plain_dist_ + std::exp( (dist - plain_dist_) / log_scale_ );
    const Float scale       = scaled_dist / dist;
    const Vector3d scaled_v    = v * scale;

    return scaled_v;
}


}





