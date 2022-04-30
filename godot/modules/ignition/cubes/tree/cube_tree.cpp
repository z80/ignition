
#include "cube_tree.h"
#include "volume_source.h"


namespace Ign
{

CubeTree::CubeTree()
{
	_max_depth = 2;
	_step      = 1.0;
}

CubeTree::~CubeTree()
{
}

void CubeTree::clear()
{
	_nodes.clear();
}

void CubeTree::compute_levels( Float size, VolumeSource * source ) const
{
	const Float min_size = source->min_node_size();
	const Float ratio = size / min_size;
	const Float levels_qty_f = std::log( ratio ) / std::log( 2.0 );
	_max_depth = static_cast<int>( std::ceil( levels_qty_f ) );
	_step      = min_size;
}

void CubeTree::subdivide( CubeTreeNode * ref_frame_physics )
{
}

bool CubeTree::parent( const CubeTreeNode & node, CubeTreeNode * & parent )
{
	if ( node.parentAbsIndex < 0 )
	{
		parent = nullptr;
		return false;
	}

	parent = &( _nodes[ node.parentAbsIndex ] );
	return true;
}

int  CubeTree::insert_node( CubeTreeNode & node )
{
}

void CubeTree::update_node( const CubeTreeNode & node )
{
}






}







