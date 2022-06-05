
#ifndef __MARCHING_CUBES_REBUILD_STRATEGY_H_
#define __MARCHING_CUBES_REBUILD_STRATEGY_H_

#include "cube_types.h"

namespace Ign
{

class MarchingCubesRebuildStrategy
{
public:
    MarchingCubesRebuildStrategy();
    ~MarchingCubesRebuildStrategy();

    bool need_rebuild( const SE3 & view_point_se3 );

    void set_rebuild_dist( Float d );
    Float get_rebuild_dist() const;

public:
    bool  initialized;
    Float rebuild_dist;
    Float rebuild_angle;
    SE3   last_se3;
};



}



#endif



