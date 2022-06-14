
#include "distance_scaler_ref.h"

namespace Ign
{

void DistanceScalerRef::_bind_methods()
{
    ClassDB::bind_method( D_METHOD( "set_plain_distance", "dist"), &DistanceScalerRef::set_plain_distance );
    ClassDB::bind_method( D_METHOD( "get_plain_distance"), &DistanceScalerRef::get_plain_distance, Variant::REAL );

    ClassDB::bind_method( D_METHOD( "scale",     "dist"), &DistanceScalerRef::scale,     Variant::REAL );
    ClassDB::bind_method( D_METHOD( "scale_v",   "v"),    &DistanceScalerRef::scale_v,   Variant::VECTOR3 );
    ClassDB::bind_method( D_METHOD( "unscale_v", "v"),    &DistanceScalerRef::unscale_v, Variant::VECTOR3 );

    ADD_PROPERTY( PropertyInfo( Variant::REAL, "plain_distance" ),    "set_plain_distance", "get_plain_distance" );
}

DistanceScalerRef::DistanceScalerRef()
    : Reference()
{
    scaler = nullptr;
}

DistanceScalerRef::~DistanceScalerRef()
{
}

void DistanceScalerRef::set_plain_distance( real_t dist )
{
    if (scaler == nullptr)
        return;
    scaler->set_plain_distance( dist );
}

real_t DistanceScalerRef::get_plain_distance() const
{
    if (scaler == nullptr)
        return 0.0;
    return scaler->plain_distance();
}

void DistanceScalerRef::set_log_scale( real_t scale )
{
    if ( scaler == nullptr )
        return;
    scaler->set_log_scale( scale );
}

real_t DistanceScalerRef::get_log_scale() const
{
    if (scaler == nullptr)
        return 0.0;
    return scaler->log_scale();
}

real_t DistanceScalerRef::scale( real_t d ) const
{
    if (scaler == nullptr)
        return d;
    return scaler->scale( d );
}

Vector3 DistanceScalerRef::scale_v( const Vector3 & v ) const
{
    if (scaler == nullptr)
        return v;
    const Vector3d vd( v.x, v.y, v.z );
    const Vector3d scaled_vd = scaler->scale( vd );
    const Vector3 ret( scaled_vd.x_, scaled_vd.y_, scaled_vd.z_ );
    return ret;
}

Vector3 DistanceScalerRef::unscale_v( const Vector3 & v ) const
{
    if (scaler == nullptr)
        return v;
    const Vector3d vd( v.x, v.y, v.z );
    const Vector3d scaled_vd = scaler->unscale( vd );
    const Vector3 ret( scaled_vd.x_, scaled_vd.y_, scaled_vd.z_ );
    return ret;
}




}



