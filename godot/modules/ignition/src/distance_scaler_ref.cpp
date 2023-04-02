
#include "distance_scaler_ref.h"

namespace Ign
{

void DistanceScalerRef::_bind_methods()
{
	ClassDB::bind_method( D_METHOD( "set_log_scale", "scale"), &DistanceScalerRef::set_log_scale );
	ClassDB::bind_method( D_METHOD( "get_log_scale"),          &DistanceScalerRef::get_log_scale );

	ADD_PROPERTY( PropertyInfo( Variant::FLOAT, "log_scale" ),         "set_log_scale",      "get_log_scale" );
}

DistanceScalerRef::DistanceScalerRef()
	: DistanceScalerBaseRef()
{
	DistanceScalerBaseRef::scaler_base = &scaler;
}

DistanceScalerRef::~DistanceScalerRef()
{
}

void DistanceScalerRef::set_log_scale( real_t scale )
{
	scaler.set_log_scale( scale );
}

real_t DistanceScalerRef::get_log_scale() const
{
	return scaler.log_scale();
}

}



