
#include "marching_Cubes_rebuild_strategy_gd.h"

namespace Ign
{

void MarchingCubesRebuildStrategyGd::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("need_rebuild", "view_point_se3"), &MarchingCubesRebuildStrategyGd::need_rebuild, Variant::BOOL );
}

MarchingCubesRebuildStrategyGd::MarchingCubesRebuildStrategyGd()
	: Reference()
{
	strategy = nullptr;
}

MarchingCubesRebuildStrategyGd::~MarchingCubesRebuildStrategyGd()
{
}

bool MarchingCubesRebuildStrategyGd::need_rebuild( const Ref<Se3Ref> & view_point_se3 )
{
	if ( strategy == nullptr )
		return false;

	const SE3 & se3 = view_point_se3.ptr()->se3;
	const bool res = strategy->need_rebuild( se3 );
	return res;
}


}


