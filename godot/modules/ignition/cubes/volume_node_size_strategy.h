
#ifndef __VOLUME_NODE_SIZE_STRATEGY_H_
#define __VOLUME_NODE_SIZE_STRATEGY_H_

#include "cube_types.h"
#include "se3.h"


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

	void set_max_node_size( Float sz );
	Float get_max_node_size() const;

	virtual Float local_node_size( const Vector3d & node_at, const Float node_size ) const;

public:
	Vector3d focal_point;
	Float    radius;
	Float    height;
};





}


#endif

