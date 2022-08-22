
#include "marching_Cubes_rebuild_strategy_gd.h"

namespace Ign
{

void MarchingCubesRebuildStrategyGd::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("need_rebuild", "view_point_se3"),          &MarchingCubesRebuildStrategyGd::need_rebuild,    Variant::BOOL );
	ClassDB::bind_method( D_METHOD("need_rescale", "view_point_se3"),          &MarchingCubesRebuildStrategyGd::need_rescale,    Variant::BOOL );
	ClassDB::bind_method( D_METHOD("local_node_size", "node_at", "node_size"), &MarchingCubesRebuildStrategyGd::local_node_size, Variant::REAL );
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

real_t MarchingCubesRebuildStrategyGd::local_node_size( const Vector3 & node_at, const real_t node_size )
{
	const real_t ret = strategy.local_node_size( Vector3d( node_at.x, node_at.y, node_at.z ), node_size );
	return ret;
}



}


