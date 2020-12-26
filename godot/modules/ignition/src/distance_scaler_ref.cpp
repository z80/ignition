
#include "distance_scaler_ref.h"

namespace Ign
{

void DistanceScalerRef::_bind_methods()
{
	ClassDB::bind_method( D_METHOD( "set_plain_distance", "dist"), &DistanceScalerRef::set_plain_distance );
	ClassDB::bind_method( D_METHOD( "get_plain_distance"), &DistanceScalerRef::get_plain_distance, Variant::REAL );

	ClassDB::bind_method( D_METHOD( "set_log_scale", "scale"), &DistanceScalerRef::set_log_scale );
	ClassDB::bind_method( D_METHOD( "get_log_scale"), &DistanceScalerRef::get_log_scale, Variant::REAL );

	ClassDB::bind_method( D_METHOD( "scale", "dist"), &DistanceScalerRef::scale, Variant::REAL );

	ADD_PROPERTY( PropertyInfo( Variant::REAL, "plain_distance" ),    "set_plain_distance", "get_plain_distance" );
	ADD_PROPERTY( PropertyInfo( Variant::REAL, "log_scale" ),         "set_log_scale",      "get_log_scale" );
}

DistanceScalerRef::DistanceScalerRef()
	: Reference()
{
}

DistanceScalerRef::~DistanceScalerRef()
{
}

void DistanceScalerRef::set_plain_distance( real_t dist )
{
	scaler.set_plain_distance( dist );
}

real_t DistanceScalerRef::get_plain_distance() const
{
	return scaler.plain_distance();
}

void DistanceScalerRef::set_log_scale( real_t scale )
{
	scaler.set_log_scale( scale );
}

real_t DistanceScalerRef::get_log_scale() const
{
	return scaler.log_scale();
}

real_t DistanceScalerRef::scale( real_t d ) const
{
	return scaler.scale( d );
}

}



