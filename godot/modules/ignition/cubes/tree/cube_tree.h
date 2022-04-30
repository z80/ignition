
#ifndef __CUBE_TREE_H_
#define __CUBE_TREE_H_

#include "data_types.h"
#include "se3.h"
#include "cube_tree_node.h"

#include <vector>

namespace Ign
{

class VolumeSource;

class CubeTree
{
public:
	CubeTree();
	~CubeTree();

	void clear();

	// Manipulating filling up the occupancy grid.
	void compute_levels( Float size, VolumeSource * source ) const;

	// These three are supposed to be used in simulation loop.
	// "subdivide" should be called once.
	void subdivide( CubeTreeNode * ref_frame_physics );

	// These three for tree construction.
	bool parent( const CubeTreeNode & node, CubeTreeNode * & parent );
	int  insert_node( CubeTreeNode & node );
	void update_node( const CubeTreeNode & node );

	std::vector<CubeTreeNode>  _nodes;

	// Maximum subdivision level.
	int   _max_depth;
	Float _step;
};


}






#endif






