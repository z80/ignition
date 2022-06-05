
#include "marching_cubes_rebuild_strategy.h"


namespace Ign
{

MarchingCubesRebuildStrategy::MarchingCubesRebuildStrategy()
{
    initialized  = false;
    rebuild_dist = 100.0;
}

~MarchingCubesRebuildStrategy()





bool MarchingCubesRebuildStrategy::need_rebuild( const SE3 & view_point_se3 )
{
    if ( !initialized )
    {
        last_se3    = source_se3;
        initialized = true;
        return true;
    }

    const SE3 camera_se3 = source_se3.inverse();
    const Float d = ( camera_se3.r_ - last_se3.r_ ).Length();
    const bool ret = ( d >= rebuild_dist );
    if ( ret )
        last_se3 = camera_se3;
}

void MarchingCubesRebuildStrategy::set_rebuild_dist( Float d )
{
    rebuild_dist = d;
}

Float MarchingCubesRebuildStrategy::get_rebuild_dist() const
{
    return rebuild_dist;
}


}












