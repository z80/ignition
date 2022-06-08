
#ifndef __MARCHING_CUBES_REBUILD_STRATEGY_H_
#define __MARCHING_CUBES_REBUILD_STRATEGY_H_

#include "cube_types.h"
#include "se3.h"

namespace Ign
{

class MarchingCubesRebuildStrategy
{
public:
	MarchingCubesRebuildStrategy();
    virtual ~MarchingCubesRebuildStrategy();

    virtual bool need_rebuild( const SE3 & view_point_se3 );


public:
    bool  initialized;
    SE3   last_se3;
};



}



#endif



