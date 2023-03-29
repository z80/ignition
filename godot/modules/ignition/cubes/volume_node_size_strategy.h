
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

	void set_max_level( int level );
	int get_max_level() const;
	int compute_max_level( Float min_detail_size );

	virtual Vector3d warp( const Vector3d & node_at ) const;

public:
	Vector3d focal_point;
	Float    radius;
	int      max_level;
};





}


#endif

