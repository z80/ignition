
#ifndef __VOLUME_NODE_SIZE_STRATEGY_GD_H_
#define __VOLUME_NODE_SIZE_STRATEGY_GD_H_

#include "volume_node_size_strategy.h"
#include "core/reference.h"
#include "se3_ref.h"

namespace Ign
{

class VolumeNodeSizeStrategyGd: public Reference
{
	GDCLASS( VolumeNodeSizeStrategyGd, Reference );
protected:
	static void _bind_methods();
public:
	VolumeNodeSizeStrategyGd();
	virtual ~VolumeNodeSizeStrategyGd();

	void set_radius( real_t r );
	real_t get_radius() const;

	void set_focal_point( const Vector3 & r );
	Vector3 get_focal_point() const;

	void set_height( real_t h );
	real_t get_height() const;

	real_t local_node_size( const Vector3 & node_at, const real_t node_size ) const;

public:
	VolumeNodeSizeStrategy strategy;
};


}


#endif


