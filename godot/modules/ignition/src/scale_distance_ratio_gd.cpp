
#include "scale_distance_ratio_gd.h"

namespace Ign
{
void ScaleDistanceRatioGd::_bind_methods()
{
	ClassDB::bind_method( D_METHOD( "set_max_distance", "dist"), &ScaleDistanceRatioGd::set_max_distance );
	ClassDB::bind_method( D_METHOD( "get_max_distance"),         &ScaleDistanceRatioGd::get_max_distance, Variant::REAL );

	ClassDB::bind_method( D_METHOD( "compute_transform", "rel_se3", "base_scale"), &ScaleDistanceRatioGd::compute_transform, Variant::TRANSFORM );

	ADD_PROPERTY( PropertyInfo( Variant::REAL, "max_distance" ),    "set_max_distance", "get_max_distance" );
}

ScaleDistanceRatioGd::ScaleDistanceRatioGd()
{
}

ScaleDistanceRatioGd::~ScaleDistanceRatioGd()
{
}

void ScaleDistanceRatioGd::set_max_distance( real_t dist )
{
	ratio.set_max_distance( dist );
}

real_t ScaleDistanceRatioGd::get_max_distance() const
{
	const real_t ret = ratio.get_max_distance();
	return ret;
}

Transform ScaleDistanceRatioGd::compute_transform( const Ref<Se3Ref> & rel_se3, real_t base_scale ) const
{
	const SE3 & se3 = rel_se3.ptr()->se3;

	Float scale;
	const SE3 scaled_se3 = ratio.compute_transform( se3, base_scale, scale );

	Transform t = scaled_se3.transform();
	t.scale( Vector3( scale, scale, scale ) );

	return t;
}

}


