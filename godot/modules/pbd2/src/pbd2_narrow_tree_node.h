
#ifndef __PBD_NARROW_TREE_NODE_H_
#define __PBD_NARROW_TREE_NODE_H_

#include "data_types.h"
#include "occupancy_types.h"


namespace Pbd
{

class NarrowTree;

class NarrowTreeNode
{
public:
	NarrowTreeNode();
	~NarrowTreeNode();
	NarrowTreeNode( const NarrowTreeNode & inst );
	const NarrowTreeNode & operator=( const NarrowTreeNode & inst );

	bool hasChildren() const;
	bool subdivide();

	// Queries.
	// Intersection with a triangle. It is for initial fill up.
	bool inside( const Face & face ) const;
        // This is for recursive collision detection.
        // Will need to extend it with "optimal" box later.
	bool inside( const NarrowTreeNode & n ) const;

	// Initialize vertices and planes.
	void init();

	NarrowTree * tree;
	int absIndex;
	int parentAbsIndex;
	int indexInParent;
	int level;

	// Child indices in Octtree list.
	int children[8];
        
        // Value is number of triangles inside.
	int      value;
	Float    size2; // Size over 2.
	Vector3d  center;
        
        Cube cube_;
        Cube cube_optimized_;

	Vector<int> ptInds;
};

}


#endif



