
#ifndef __PBD_BROAD_TREE_H_
#define __PBD_BROAD_TREE_H_

#include "data_types.h"
#include "pbd2_broad_tree_node.h"



namespace Pbd
{

/**
This data structure is for determining empty/filled space only.
Nonempty leaf node means space is filled. Else means it's empty.
**/

class NarrowTree;
class NarrowTreeNode;

class BroadTree
{
public:
	BroadTree();
	~BroadTree();

	// Manipulating filling up the occupancy grid.
	void set_node_size( Float sz = 0.1 );
	Float node_size() const;
	void clear();
	void append( const Transform & t, const Ref<Mesh> & mesh );
	void subdivide();

	// Internally called for recursion.
	bool node_intersects( const BroadTreeNode & n, const BroadTree & tree ) const;

	// For visualization.
	PoolVector<Vector3> lines();
    
        // These three for tree construction.
	bool parent( const BroadTreeNode & node, BroadTreeNode * & parent );
	int  insert_node( BroadTreeNode & node );
	void update_node( const BroadTreeNode & node );

	Vector<BroadTreeNode> nodes_;
	Vector<Face>          faces_;

	// Maximum subdivision level.
	Float node_sz_;
	int   max_depth_;
        int   min_triangles_;
};


}






#endif





