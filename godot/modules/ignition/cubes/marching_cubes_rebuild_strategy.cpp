
#include "marching_cubes_rebuild_strategy.h"


namespace Ign
{

MarchingCubesRebuildStrategy::MarchingCubesRebuildStrategy()
{
    initialized  = false;
}

MarchingCubesRebuildStrategy::~MarchingCubesRebuildStrategy()
{
}



bool MarchingCubesRebuildStrategy::need_rebuild( const SE3 & view_point_se3 )
{
    if ( !initialized )
    {
        last_se3    = view_point_se3;
        initialized = true;
        return true;
    }

	return false;
}



}












