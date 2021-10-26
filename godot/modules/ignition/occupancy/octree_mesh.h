
#ifndef __OCTREE_MESH_H_
#define __OCTREE_MESH_H_

#include "core/vector.h"
#include "core/math/face3.h"
#include "scene/main/node.h"
#include "scene/3d/mesh_instance.h"
#include "octree_mesh_node.h"


/**
This data structure is for determining empty/filled space only.
Nonempty leaf node means space is filled. Else means it's empty.
**/

class OctreeMesh
{
public:
	OctreeMesh();
	~OctreeMesh();

	// Manipulating filling up the occupancy grid.
	void set_node_size( real_t sz = 0.1 );
	real_t node_size() const;
	void clear();
	void append( const Transform & t, const Ref<Mesh> mesh );
	void subdivide();

	// Check if certain point is occupied.
	bool occupied( const Vector3 & at ) const;
	// Internally called for recursion.
	bool point_inside( const OctreeMeshNode & n, const Vector3 & at ) const;
	// Point ajacent.
	bool point_ajacent( const Vector3 & at ) const;
	// Intersects.
	bool intersects( const OctreeMesh * tree ) const;
	// Internally called for recursion.
	bool node_intersects( const OctreeMeshNode & n, const OctreeMesh & tree ) const;
	// Touches.
	bool touches( const OctreeMesh * tree ) const;
	Vector3 touch_point( const OctreeMesh * tree ) const;
	// Intersects with infinite ray.
	bool intersects_ray( const Vector3 origin, const Vector3 dir ) const;

	// Moving entire tree to a different location.
	void set_position( const Vector3 & at );
	Vector3 get_position() const;
	// Internally used for recursive calls.
	void set_node_position( OctreeMeshNode & n, const Vector3 from, const Vector3 to );

	// For visualization.
	PoolVector<Vector3> lines();

	bool parent( const OctreeMeshNode & node, OctreeMeshNode * & parent );

	int  insert_node( OctreeMeshNode & node );
	void update_node( const OctreeMeshNode & node );

	Vector<OctreeMeshNode> nodes_;
	Vector<Face3>          faces_;

	// Maximum subdivision level.
	real_t node_sz_;
	int    max_depth_;
};







#endif





