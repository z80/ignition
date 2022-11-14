
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

	void clear_node_sizes();
	void set_node_sizes( const Array & distances, const Array & node_sizes );

public:
	VolumeNodeSizeStrategy strategy;
};


}


#endif


