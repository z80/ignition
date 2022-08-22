
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

	void init( Float planet_radius, Float height, Float rescale_dist, Float rebuild_dist );

    virtual bool need_rebuild( const SE3 & view_point_se3 );
    virtual bool need_rescale( const SE3 & view_point_se3 );
	virtual Float local_node_size( const Vector3d & node_at, const Float node_size ) const;


public:
    Vector3d focus;
	Float    height, planet_radius;
	Float    rescale_dist, rebuild_dist;
};



}



#endif



