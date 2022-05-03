
#ifndef __CUBE_TREE_H_
#define __CUBE_TREE_H_

#include "data_types.h"
#include "se3.h"
#include "cube_tree_node.h"

#include <vector>

namespace Ign
{

class MarchingVolumeObject;

class CubeTree
{
public:
	CubeTree();
	virtual ~CubeTree();

	void set_se3( const SE3 & se3 );
	const SE3 & get_se3() const;


	virtual void fill_source_references();

	// Manipulating filling up the occupancy grid.
	void compute_levels( Float total_max_size );

	// These three are supposed to be used in simulation loop.
	// "subdivide" should be called once.
	void subdivide( Float total_max_size=-1.0 );

	// These three for tree construction.
	bool parent( const CubeTreeNode & node, CubeTreeNode * & parent );
	int  insert_node( CubeTreeNode & node );
	void update_node( const CubeTreeNode & node );

	Vector3d at( const VectorInt & at_i ) const;


	SE3 se3;
	SE3 se3_inverted;


	std::vector<MarchingVolumeObject *> sources;
	std::vector<CubeTreeNode>           nodes;

	// Maximum subdivision level.
	int   max_depth;
	Float step;
};


}






#endif






