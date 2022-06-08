
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

public:
	MarchingCubesRebuildStrategy * strategy;
};


}


#endif



