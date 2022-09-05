
#include "marching_cubes_rebuild_strategy_gd.h"

namespace Ign
{

void MarchingCubesRebuildStrategyGd::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("need_rebuild", "view_point_se3"),          &MarchingCubesRebuildStrategyGd::need_rebuild,    Variant::BOOL );
	ClassDB::bind_method( D_METHOD("need_rescale", "view_point_se3"),          &MarchingCubesRebuildStrategyGd::need_rescale,    Variant::BOOL );

	ClassDB::bind_method( D_METHOD("get_focal_point_rebuild"), &MarchingCubesRebuildStrategyGd::get_focal_point_rebuild, Variant::VECTOR3 );
	ClassDB::bind_method( D_METHOD("get_focal_point_rescale"), &MarchingCubesRebuildStrategyGd::get_focal_point_rescale, Variant::VECTOR3 );

	ClassDB::bind_method( D_METHOD("set_radius", "radius"), &MarchingCubesRebuildStrategyGd::set_radius );
	ClassDB::bind_method( D_METHOD("get_radius"),           &MarchingCubesRebuildStrategyGd::get_radius, Variant::REAL );

	ClassDB::bind_method( D_METHOD("set_height", "height"), &MarchingCubesRebuildStrategyGd::set_height );
	ClassDB::bind_method( D_METHOD("get_height"),           &MarchingCubesRebuildStrategyGd::get_height, Variant::REAL );

	ClassDB::bind_method( D_METHOD("set_rescale_dist", "dist"), &MarchingCubesRebuildStrategyGd::set_rescale_dist );
	ClassDB::bind_method( D_METHOD("get_rescale_dist"),         &MarchingCubesRebuildStrategyGd::get_rescale_dist, Variant::REAL );

	ClassDB::bind_method( D_METHOD("set_rebuild_dist", "dist"), &MarchingCubesRebuildStrategyGd::set_rebuild_dist );
	ClassDB::bind_method( D_METHOD("get_rebuild_dist"),         &MarchingCubesRebuildStrategyGd::get_rebuild_dist, Variant::REAL );

	ADD_PROPERTY( PropertyInfo( Variant::REAL, "radius" ),       "set_radius",       "get_radius" );
	ADD_PROPERTY( PropertyInfo( Variant::REAL, "height" ),       "set_height",       "get_height" );
	ADD_PROPERTY( PropertyInfo( Variant::REAL, "rescale_dist" ), "set_rescale_dist", "get_rescale_dist" );
	ADD_PROPERTY( PropertyInfo( Variant::REAL, "rebuild_dist" ), "set_rebuild_dist", "get_rebuild_dist" );
}

MarchingCubesRebuildStrategyGd::MarchingCubesRebuildStrategyGd()
	: Reference()
{
}

MarchingCubesRebuildStrategyGd::~MarchingCubesRebuildStrategyGd()
{
}

bool MarchingCubesRebuildStrategyGd::need_rebuild( const Ref<Se3Ref> & view_point_se3 )
{
	const SE3 & se3 = view_point_se3.ptr()->se3;
	const bool res = strategy.need_rebuild( se3 );
	return res;
}

bool MarchingCubesRebuildStrategyGd::need_rescale( const Ref<Se3Ref> & view_point_se3 )
{
	const SE3 & se3 = view_point_se3.ptr()->se3;
	const bool res = strategy.need_rescale( se3 );
	return res;
}

Vector3 MarchingCubesRebuildStrategyGd::get_focal_point_rebuild() const
{
	const Vector3d v = strategy.get_focal_point_rebuild();
	const Vector3 ret( v.x_, v.y_, v.z_ );
	return ret;
}

Vector3 MarchingCubesRebuildStrategyGd::get_focal_point_rescale() const
{
	const Vector3d v = strategy.get_focal_point_rescale();
	const Vector3 ret( v.x_, v.y_, v.z_ );
	return ret;
}

void MarchingCubesRebuildStrategyGd::set_radius( real_t radius )
{
	strategy.set_radius( radius );
}

real_t MarchingCubesRebuildStrategyGd::get_radius() const
{
	const real_t ret = strategy.get_radius();
	return ret;
}


void MarchingCubesRebuildStrategyGd::set_height( real_t height )
{
	strategy.set_height( height );
}

real_t MarchingCubesRebuildStrategyGd::get_height() const
{
	const real_t ret = strategy.get_height();
	return ret;
}


void MarchingCubesRebuildStrategyGd::set_rescale_dist( real_t dist )
{
	strategy.set_rescale_dist( dist );
}

real_t MarchingCubesRebuildStrategyGd::get_rescale_dist() const
{
	const real_t ret = strategy.get_rescale_dist();
	return ret;
}

void MarchingCubesRebuildStrategyGd::set_rebuild_dist( real_t dist )
{
	strategy.set_rebuild_dist( dist );
}

real_t MarchingCubesRebuildStrategyGd::get_rebuild_dist() const
{
	const real_t ret = strategy.get_rebuild_dist();
	return ret;
}




}


