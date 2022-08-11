
#ifndef __MARCHING_CUBES_SPHERICAL_REBUILD_STRATEGY_H_
#define __MARCHING_CUBES_SPHERICAL_REBUILD_STRATEGY_H_

#include "marching_cubes_rebuild_strategy.h"
#include "cube_types.h"
#include "se3.h"

namespace Ign
{

class DistanceScalerBase;

class MarchingCubesSphericalRebuildStrategy: public MarchingCubesRebuildStrategy
{
public:
    MarchingCubesSphericalRebuildStrategy();
    virtual ~MarchingCubesSphericalRebuildStrategy();
    
    void initialize( Float radius, const DistanceScalerBase * scaler );

	void set_rebuild_angle( Float ang );
	Float get_rebuild_angle() const;

    virtual bool need_rebuild( const SE3 & view_point_se3 ) override;


public:
    Float rebuild_angle;
};



}



#endif



