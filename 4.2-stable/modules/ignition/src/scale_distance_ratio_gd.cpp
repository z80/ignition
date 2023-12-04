
#include "scale_distance_ratio_gd.h"

namespace Ign
{
void ScaleDistanceRatioGd::_bind_methods()
{
	ClassDB::bind_method( D_METHOD( "set_max_distance", "dist"), &ScaleDistanceRatioGd::set_max_distance );
	ClassDB::bind_method( D_METHOD( "get_max_distance"),         &ScaleDistanceRatioGd::get_max_distance );

	ClassDB::bind_method( D_METHOD( "compute_scale",     "rel_se3", "base_scale"), &ScaleDistanceRatioGd::compute_scale );
	ClassDB::bind_method( D_METHOD( "compute_transform", "rel_se3", "base_scale"), &ScaleDistanceRatioGd::compute_transform );

	ADD_PROPERTY( PropertyInfo( Variant::FLOAT, "max_distance" ),    "set_max_distance", "get_max_distance" );
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

real_t ScaleDistanceRatioGd::compute_scale( const Ref<Se3Ref> & rel_se3, real_t base_scale ) const
{
	const SE3 & se3 = rel_se3.ptr()->se3;

	const Float scale = ratio.compute_scale( se3, base_scale );
	return scale;
}

Transform3D ScaleDistanceRatioGd::compute_transform( const Ref<Se3Ref> & rel_se3, real_t base_scale ) const
{
	const SE3 & se3 = rel_se3.ptr()->se3;

	const Float scale = ratio.compute_scale( se3, base_scale );

	Transform3D t;

	t.basis = Basis( se3.q() );
	t.scale( Vector3( scale, scale, scale ) );

	const Vector3d origin = se3.r_ * (base_scale * scale);
	t.origin = Vector3( origin.x_, origin.y_, origin.z_ );

	return t;
}

}


