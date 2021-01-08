
#ifndef __OCCUPANCY_TREE_H_
#define __OCCUPANCY_TREE_H_

#include "data_types.h"
#include "occupancy_tree_node.h"

#include "scene/3d/mesh_instance.h"


namespace Pbd
{

/**
This data structure is for determining empty/filled space only.
Nonempty leaf node means space is filled. Else means it's empty.
**/

class OccupancyTree
{
public:
	OccupancyTree();
	~OccupancyTree();

	// Manipulating filling up the occupancy grid.
	void set_node_size( Float sz = 0.1 );
	Float node_size() const;
	void clear();
	void append( const Transform & t, const Ref<Mesh> & mesh );
	void subdivide();

	// Check if certain point is occupied.
	bool occupied( const Vector3 & at ) const;
	// Internally called for recursion.
	bool point_inside( const OccupancyTreeNode & n, const Vector3 & at ) const;
	// Point ajacent.
	bool point_ajacent( const Vector3 & at ) const;
	// Intersects.
	bool intersects( const OccupancyTree * tree ) const;
	// Internally called for recursion.
	bool node_intersects( const OccupancyTreeNode & n, const OccupancyTree & tree ) const;
	// Touches.
	bool touches( const OccupancyTree * tree ) const;
	Vector3 touch_point( const OccupancyTree * tree ) const;
	// Intersects with infinite ray.
	bool intersects_ray( const Vector3 p_from, const Vector3 p_to ) const;

	// Moving entire tree to a different location.
	void set_position( const Vector3 & at );
	Vector3 get_position() const;
	// Internally used for recursive calls.
	void set_node_position( OccupancyTreeNode & n, const Vector3 from, const Vector3 to );

	// For visualization.
	PoolVector<Vector3> lines();

	bool parent( const OccupancyTreeNode & node, OccupancyTreeNode * & parent );

	int  insert_node( OccupancyTreeNode & node );
	void update_node( const OccupancyTreeNode & node );

	Vector<OccupancyTreeNode> nodes_;
	Vector<Face3>             faces_;

	// Maximum subdivision level.
	Float node_sz_;
	int   max_depth_;
};


}






#endif





