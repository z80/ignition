
#ifndef __MARCHING_CUBES_REBUILD_STRATEGY_GD_H_
#define __MARCHING_CUBES_REBUILD_STRATEGY_GD_H_

#include "marching_cubes_rebuild_strategy.h"
#include "core/reference.h"
#include "se3_ref.h"

namespace Ign
{

class MarchingCubesRebuildStrategyGd: public Reference
{
	GDCLASS( MarchingCubesRebuildStrategyGd, Reference );
protected:
	static void _bind_methods();
public:
	MarchingCubesRebuildStrategyGd();
	virtual ~MarchingCubesRebuildStrategyGd();

	void init( real_t height, real_t planet_radius, real_t rescale_dist, real_t rebuild_dist );
	bool need_rebuild( const Ref<Se3Ref> & view_point_se3 );
	bool need_rescale( const Ref<Se3Ref> & view_point_se3 );
	real_t local_node_size( const Vector3 & node_at, const real_t node_size );

public:
	MarchingCubesRebuildStrategy strategy;
};


}


#endif



