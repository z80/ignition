
#ifndef __PBD_OCCUPANCY_TREE_H_
#define __PBD_OCCUPANCY_TREE_H_

#include "data_types.h"
#include "se3.h"
#include "pbd2_narrow_tree_sdf_node.h"
#include "pbd2_narrow_tree_pts_node.h"

#include "scene/3d/mesh_instance.h"


namespace Pbd
{

/**
This data structure is for determining empty/filled space only.
Nonempty leaf node means space is filled. Else means it's empty.
**/

class NarrowTree
{
public:
	NarrowTree();
	~NarrowTree();

	// Manipulating filling up the occupancy grid.
	void set_max_level( int new_level=2 );
	int max_level() const;
	void clear();
	void append( const Transform & t, const Ref<Mesh> & mesh );
	void append_triangle( const Vector3d & a, const Vector3d & b, const Vector3d & c );
	// "subdivide" should be called.
	void subdivide();
	// These two are used by the upper one.
	// These shouldn't be used individually.
	void subdivide_sdf();
	void subdivide_pts();

	void apply( const SE3 & se3 );

    bool intersects( NarrowTree * tree, Vector<Vector3d> & pts, Vector<Vector3d> & depths ) const;

	// For visualization.
	PoolVector3Array lines_sdf_nodes() const;
	PoolVector3Array lines_surface_pts() const;
	PoolVector3Array lines_pts_nodes() const;
	PoolVector3Array lines_aligned_nodes() const;

    // These three for tree construction.
	bool parent_sdf( const NarrowTreeSdfNode & node, NarrowTreeSdfNode * & parent );
	bool parent_pts( const NarrowTreePtsNode & node, NarrowTreePtsNode * & parent );
	int  insert_node_sdf( NarrowTreeSdfNode & node );
	int  insert_node_pts( NarrowTreePtsNode & node );
	void update_node_sdf( const NarrowTreeSdfNode & node );
	void update_node_pts( const NarrowTreePtsNode & node );

	SE3                       se3_;
	Vector<NarrowTreeSdfNode> nodes_sdf_;
	Vector<NarrowTreePtsNode> nodes_pts_;
	Vector<Face>              faces_;
	Vector<Vector3d>          pts_;

	// Unconditionally subdivide to this level.
	int   min_depth_;
	// Subdivide SDF node if it's error is bigger than this.
	Float max_sdf_error_;
	// Unconditionally stop subdividing if level has reached this value.
	// Maximum subdivision level.
	int max_depth_;
	// Min points in a node while keeping subdividing.
	int min_pts_;
};


}






#endif





