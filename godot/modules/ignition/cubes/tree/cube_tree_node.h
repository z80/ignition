
#ifndef __CUBE_TREE_NODE_H_
#define __CUBE_TREE_NODE_H_

#include "cube_types.h"
#include <vector>


namespace Ign
{

class CubeTree;
class MarchingVolumeObject;

class CubeTreeNode
{
public:
	CubeTreeNode();
	~CubeTreeNode();
	CubeTreeNode( const CubeTreeNode & inst );
	const CubeTreeNode & operator=( const CubeTreeNode & inst );

	bool has_children() const;
	bool subdivide( CubeTree * tree );

	// Initialize vertices and planes.
	void init( CubeTree * tree );

public:
	int abs_index;
	int parent_abs_index;
	int index_in_parent;

	// Child indices in Octtree list.
	int children[8];

	int       size; // Size.
	VectorInt corner;

	// For quick computations.
	Vector3d corner_min;
	Vector3d corner_max;

	std::vector<int> source_inds;
};


}




#endif












