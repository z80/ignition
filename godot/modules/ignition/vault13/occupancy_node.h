
#ifndef __OCCUPANCY_NODE_H_
#define __OCCUPANCY_NODE_H_

#include "scene/main/node.h"
#include "occupancy_tree.h"

class OccupancyNode: public Node
{
	GDCLASS(OccupancyNode, Node);
	OBJ_CATEGORY("Ignition");

protected:
	static void _bind_methods();

public:
	OccupancyNode();
	~OccupancyNode();

	// Manipulating filling up the occupancy grid.
	void set_node_size( real_t sz );
	real_t node_size() const;
	void rebuild();
	// For visualization.
	Vector<Vector3> lines();

	// Check if certain point is occupied.
	bool occupied( const Vector3 & at ) const;
	// Point ajacent.
	bool point_ajacent( const Vector3 & at ) const;
	// Intersects.
	bool intersects( Node * node ) const;
	// Touches.
	bool touches( Node * node ) const;
	Vector3 touch_point( Node * node ) const;
	// Intersects with infinite ray.
	bool intersects_ray( const Vector3 p_from, const Vector3 p_to ) const;



	OccupancyTree _tree;
};




#endif



