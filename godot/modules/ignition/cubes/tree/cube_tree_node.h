
#ifndef __CUBE_TREE_NODE_H_
#define __CUBE_TREE_NODE_H_

#include "cube_types.h"


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

	bool inside( CubeTree * tree, const MarchingVolumeObject * volume_obj ) const;


	// Initialize vertices and planes.
	void init();

public:
	int abs_index;
	int parent_abs_index;
	int index_in_parent;
	int level;

	// Child indices in Octtree list.
	int children[8];

	int       size; // Size over 2.
	VectorInt corner;

	std::vector<int> object_inds;
};


}




#endif












