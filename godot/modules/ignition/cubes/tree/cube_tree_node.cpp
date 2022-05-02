
#include "cube_tree_node.h"
#include "cube_tree.h"
#include "marching_volume_object.h"


namespace Ign
{

CubeTreeNode::CubeTreeNode()
{
	parent_abs_index = -1;
	index_in_parent  = -1;
	level            = -1;
	abs_index        = -1;

	for ( int i=0; i<8; i++ )
		children[i] = -1;

	size   = 1;
	corner = VectorInt( 0, 0, 0 );
}

CubeTreeNode::~CubeTreeNode()
{
}

CubeTreeNode::CubeTreeNode( const CubeTreeNode & inst )
{
	*this = inst;
}

const CubeTreeNode & CubeTreeNode::operator=( const CubeTreeNode & inst )
{
	if ( this != &inst )
	{
		parent_abs_index = inst.parent_abs_index;
		index_in_parent  = inst.index_in_parent;
		level            = inst.level;
		abs_index        = inst.abs_index;

		children[0] = inst.children[0];
		children[1] = inst.children[1];
		children[2] = inst.children[2];
		children[3] = inst.children[3];
		children[4] = inst.children[4];
		children[5] = inst.children[5];
		children[6] = inst.children[6];
		children[7] = inst.children[7];

		size   = inst.size;
		corner = inst.corner;

		object_inds = inst.object_inds;

		init();
	}

	return *this;
}

bool CubeTreeNode::has_children() const
{
	// Filled can't have children.
	if ( !obj_inds.empty() )
		return false;

	for ( int i=0; i<8; i++ )
	{
		if ( children[i] >= 0 )
			return true;
	}

	return false;
}

bool CubeTreeNode::subdivide( CubeTree * tree )
{
}

bool CubeTreeNode::inside( CubeTree * tree, const MarchingVolumeObject * volume_obj ) const
{
}

void CubeTreeNode::init()
{
}





}


