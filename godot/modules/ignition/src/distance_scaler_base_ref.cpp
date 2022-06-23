
#include "distance_scaler_base_ref.h"

namespace Ign
{

void DistanceScalerBaseRef::_bind_methods()
{
    ClassDB::bind_method( D_METHOD( "set_plain_distance", "dist"), &DistanceScalerBaseRef::set_plain_distance );
    ClassDB::bind_method( D_METHOD( "get_plain_distance"), &DistanceScalerBaseRef::get_plain_distance, Variant::REAL );

    ClassDB::bind_method( D_METHOD( "scale",     "dist"), &DistanceScalerBaseRef::scale,     Variant::REAL );
    ClassDB::bind_method( D_METHOD( "scale_v",   "v"),    &DistanceScalerBaseRef::scale_v,   Variant::VECTOR3 );
    ClassDB::bind_method( D_METHOD( "unscale_v", "v"),    &DistanceScalerBaseRef::unscale_v, Variant::VECTOR3 );

    ADD_PROPERTY( PropertyInfo( Variant::REAL, "plain_distance" ),    "set_plain_distance", "get_plain_distance" );
}

DistanceScalerBaseRef::DistanceScalerBaseRef()
    : Reference()
{
    scaler = nullptr;
}

DistanceScalerBaseRef::~DistanceScalerBaseRef()
{
}

void DistanceScalerBaseRef::set_plain_distance( real_t dist )
{
    if (scaler == nullptr)
        return;
    scaler->set_plain_distance( dist );
}

real_t DistanceScalerBaseRef::get_plain_distance() const
{
    if (scaler == nullptr)
        return 0.0;
    return scaler->plain_distance();
}

real_t DistanceScalerBaseRef::scale( real_t d ) const
{
    if (scaler == nullptr)
        return d;
    return scaler->scale( d );
}

Vector3 DistanceScalerBaseRef::scale_v( const Vector3 & v ) const
{
    if (scaler == nullptr)
        return v;
    const Vector3d vd( v.x, v.y, v.z );
    const Vector3d scaled_vd = scaler->scale( vd );
    const Vector3 ret( scaled_vd.x_, scaled_vd.y_, scaled_vd.z_ );
    return ret;
}

Vector3 DistanceScalerBaseRef::unscale_v( const Vector3 & v ) const
{
    if (scaler == nullptr)
        return v;
    const Vector3d vd( v.x, v.y, v.z );
    const Vector3d scaled_vd = scaler->unscale( vd );
    const Vector3 ret( scaled_vd.x_, scaled_vd.y_, scaled_vd.z_ );
    return ret;
}




}



