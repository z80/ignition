
#include "marching_cubes_rebuild_strategy_gd.h"

namespace Ign
{

void MarchingCubesRebuildStrategyGd::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("need_rebuild", "view_point_se3"),          &MarchingCubesRebuildStrategyGd::need_rebuild );
	ClassDB::bind_method( D_METHOD("need_rescale", "view_point_se3"),          &MarchingCubesRebuildStrategyGd::need_rescale );

	ClassDB::bind_method( D_METHOD("get_focal_point_rebuild"), &MarchingCubesRebuildStrategyGd::get_focal_point_rebuild );
	ClassDB::bind_method( D_METHOD("get_focal_point_rescale"), &MarchingCubesRebuildStrategyGd::get_focal_point_rescale );

	ClassDB::bind_method( D_METHOD("set_radius", "radius"), &MarchingCubesRebuildStrategyGd::set_radius );
	ClassDB::bind_method( D_METHOD("get_radius"),           &MarchingCubesRebuildStrategyGd::get_radius );

	ClassDB::bind_method( D_METHOD("set_height", "height"), &MarchingCubesRebuildStrategyGd::set_height );
	ClassDB::bind_method( D_METHOD("get_height"),           &MarchingCubesRebuildStrategyGd::get_height );

	ClassDB::bind_method( D_METHOD("set_rebuild_dist", "dist"), &MarchingCubesRebuildStrategyGd::set_rebuild_dist );
	ClassDB::bind_method( D_METHOD("get_rebuild_dist"),         &MarchingCubesRebuildStrategyGd::get_rebuild_dist );

	ClassDB::bind_method( D_METHOD("set_rescale_close_dist", "dist"), &MarchingCubesRebuildStrategyGd::set_rescale_close_dist );
	ClassDB::bind_method( D_METHOD("get_rescale_close_dist"),         &MarchingCubesRebuildStrategyGd::get_rescale_close_dist );

	ClassDB::bind_method( D_METHOD("set_rescale_far_tangent", "dist"), &MarchingCubesRebuildStrategyGd::set_rescale_far_tangent );
	ClassDB::bind_method( D_METHOD("get_rescale_far_tangent"),         &MarchingCubesRebuildStrategyGd::get_rescale_far_tangent );

	ClassDB::bind_method( D_METHOD("set_rescale_depth_rel_tangent", "dist"), &MarchingCubesRebuildStrategyGd::set_rescale_depth_rel_tangent );
	ClassDB::bind_method( D_METHOD("get_rescale_depth_rel_tangent"),         &MarchingCubesRebuildStrategyGd::get_rescale_depth_rel_tangent );

	ADD_GROUP( "Ignition", "" );
	ADD_PROPERTY( PropertyInfo( Variant::FLOAT, "radius" ),       "set_radius",       "get_radius" );
	ADD_PROPERTY( PropertyInfo( Variant::FLOAT, "height" ),       "set_height",       "get_height" );
	ADD_PROPERTY( PropertyInfo( Variant::FLOAT, "rebuild_dist" ), "set_rebuild_dist", "get_rebuild_dist" );
	ADD_PROPERTY( PropertyInfo( Variant::FLOAT, "rescale_close_dist" ),        "set_rescale_close_dist",        "get_rescale_close_dist" );
	ADD_PROPERTY( PropertyInfo( Variant::FLOAT, "rescale_far_tangent" ),       "set_rescale_far_tangent",       "get_rescale_far_tangent" );
	ADD_PROPERTY( PropertyInfo( Variant::FLOAT, "rescale_depth_rel_tangent" ), "set_rescale_depth_rel_tangent", "get_rescale_depth_rel_tangent" );
}

MarchingCubesRebuildStrategyGd::MarchingCubesRebuildStrategyGd()
	: RefCounted()
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

void MarchingCubesRebuildStrategyGd::set_rebuild_dist( real_t dist )
{
	strategy.set_rebuild_dist( dist );
}

real_t MarchingCubesRebuildStrategyGd::get_rebuild_dist() const
{
	const real_t ret = strategy.get_rebuild_dist();
	return ret;
}

void MarchingCubesRebuildStrategyGd::set_rescale_close_dist( real_t dist )
{
	strategy.set_rescale_close_dist( dist );
}

real_t MarchingCubesRebuildStrategyGd::get_rescale_close_dist() const
{
	const real_t ret = strategy.get_rescale_close_dist();
	return ret;
}

void MarchingCubesRebuildStrategyGd::set_rescale_far_tangent( real_t dist )
{
	strategy.set_rescale_far_tangent( dist );
}

real_t MarchingCubesRebuildStrategyGd::get_rescale_far_tangent() const
{
	const real_t ret = strategy.get_rescale_far_tangent();
	return ret;
}

void MarchingCubesRebuildStrategyGd::set_rescale_depth_rel_tangent( real_t dist )
{
	strategy.set_rescale_depth_rel_tangent( dist );
}

real_t MarchingCubesRebuildStrategyGd::get_rescale_depth_rel_tangent() const
{
	const real_t ret = strategy.get_rescale_depth_rel_tangent();
	return ret;
}




}


