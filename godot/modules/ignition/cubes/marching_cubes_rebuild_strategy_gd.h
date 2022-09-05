
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

	bool need_rebuild( const Ref<Se3Ref> & view_point_se3 );
	bool need_rescale( const Ref<Se3Ref> & view_point_se3 );

	void     set_radius( real_t radius );
	real_t   get_radius() const;

	void     set_height( real_t height );
	real_t   get_height() const;

	void     set_rescale_dist( real_t dist );
	real_t   get_rescale_dist() const;

	void     set_rebuild_dist( real_t dist );
	real_t   get_rebuild_dist() const;

public:
	MarchingCubesRebuildStrategy strategy;
};


}


#endif



