
#ifndef __PBD_OCCUPANCY_TREE_H_
#define __PBD_OCCUPANCY_TREE_H_

#include "data_types.h"
#include "se3.h"
#include "pbd2_narrow_tree_node.h"

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
	void set_node_size( Float sz = 0.1 );
	Float node_size() const;
	void clear();
	void append( const Transform & t, const Ref<Mesh> & mesh );
	void subdivide();

        bool intersects( const NarrowTree * tree ) const;
	// Internally called for recursion.
	bool node_intersects( const NarrowTreeNode & n, const NarrowTree & tree ) const;

	// For visualization.
	PoolVector<Vector3> lines();
    
        // These three for tree construction.
	bool parent( const NarrowTreeNode & node, NarrowTreeNode * & parent );
	int  insert_node( NarrowTreeNode & node );
	void update_node( const NarrowTreeNode & node );

	Vector<NarrowTreeNode> nodes_;
	Vector<Face>           faces_;

	// Maximum subdivision level.
	Float node_sz_;
	int   max_depth_;
        int   min_triangles_;
};


}






#endif





