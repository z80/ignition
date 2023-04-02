
#ifndef __VOLUME_NODE_SIZE_STRATEGY_GD_H_
#define __VOLUME_NODE_SIZE_STRATEGY_GD_H_

#include "volume_node_size_strategy.h"
#include "core/object/ref_counted.h"
#include "se3_ref.h"

namespace Ign
{

class VolumeNodeSizeStrategyGd: public RefCounted
{
	GDCLASS( VolumeNodeSizeStrategyGd, RefCounted );
protected:
	static void _bind_methods();
public:
	VolumeNodeSizeStrategyGd();
	virtual ~VolumeNodeSizeStrategyGd();

	void set_radius( real_t r );
	real_t get_radius() const;

	void set_focal_point( const Vector3 & r );
	Vector3 get_focal_point() const;

	void set_max_level( int level );
	int get_max_level() const;
	int compute_max_level( real_t min_detail_size );

public:
	VolumeNodeSizeStrategy strategy;
};


}


#endif


