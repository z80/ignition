
#ifndef __MARCHING_CUBES_SPHERICAL_REBUILD_STRATEGY_GD_H_
#define __MARCHING_CUBES_SPHERICAL_REBUILD_STRATEGY_GD_H_

#include "marching_cubes_rebuild_strategy_gd.h"
#include "marching_cubes_spherical_rebuild_strategy.h"

#include "distance_scaler_base_ref.h"

namespace Ign
{

class MarchingCubesSphericalRebuildStrategyGd: public MarchingCubesRebuildStrategyGd
{
	GDCLASS( MarchingCubesSphericalRebuildStrategyGd, MarchingCubesRebuildStrategyGd );
protected:
	static void _bind_methods();
public:
	MarchingCubesSphericalRebuildStrategyGd();
	virtual ~MarchingCubesSphericalRebuildStrategyGd();
	
	void initialize( real_t radius, const Ref<DistanceScalerBaseRef> & scaler );

	void set_rebuild_angle( real_t ang );
	real_t get_rebuild_angle() const;

public:
	MarchingCubesSphericalRebuildStrategy spherical_strategy;
};



}



#endif



