
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
    virtual bool need_rescale( const SE3 & view_point_se3 );

	Vector3d get_focal_point_rebuild() const;
	Vector3d get_focal_point_rescale() const;

	void     set_radius( Float radius );
	Float    get_radius() const;

	void     set_height( Float height );
	Float    get_height() const;

	void     set_rescale_dist( Float dist );
	Float    get_rescale_dist() const;

	void     set_rebuild_dist( Float dist );
	Float    get_rebuild_dist() const;

public:
    Vector3d focal_point_rebuild;
	Vector3d focal_point_rescale;
	Float    height, planet_radius;
	Float    rescale_dist, rebuild_dist;
};



}



#endif



