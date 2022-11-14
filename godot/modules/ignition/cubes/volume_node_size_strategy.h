
#ifndef __VOLUME_NODE_SIZE_STRATEGY_H_
#define __VOLUME_NODE_SIZE_STRATEGY_H_

#include "cube_types.h"
#include "se3.h"

#include <vector>

namespace Ign
{

class VolumeNodeSizeStrategy
{
public:
	VolumeNodeSizeStrategy();
	virtual ~VolumeNodeSizeStrategy();

	void set_radius( Float r );
	Float get_radius() const;

	void set_focal_point( const Vector3d & r );
	Vector3d get_focal_point() const;

	void set_height( Float h );
	Float get_height() const;

	void clear_node_sizes();
	void append_node_size( Float distance, Float node_size );

	virtual Float local_node_size( const Vector3d & node_at, const Float node_size ) const;
	virtual bool can_subdivide( const Vector3d & node_at, const Float node_size, const Float min_node_size ) const;

public:
	Vector3d focal_point;
	Float    radius;
	Float    min_distance;

	struct NodeSizeAtDistance
	{
		Float distance;
		Float node_size;
	};
	std::vector<NodeSizeAtDistance> node_size_of_distance;
};





}


#endif

