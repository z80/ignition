
#include "cube_tree.h"
#include "marching_volume_object.h"


namespace Ign
{

CubeTree::CubeTree()
{
	max_depth = 2;
	step      = 1.0;
}

CubeTree::~CubeTree()
{
}

void CubeTree::set_se3( const SE3 & se3 )
{
	this->se3 = se3;
	this->se3_inverted = se3.inverse();
}

const SE3 & CubeTree::get_se3() const
{
	return se3;
}

void CubeTree::fill_source_references()
{
	sources.clear();
}

void CubeTree::compute_levels( Float total_max_size )
{
	Float max_sz = -1.0;
	for ( std::vector<MarchingVolumeObject *>::const_iterator it=sources.begin(); it!=sources.end(); it++ )
	{
		const MarchingVolumeObject * obj = *it;
		const Vector3d at = obj->at();
		const Float    sz = obj->get_bounding_radius();
		// If object doesn't have bounding radius like
		// a plane, ignore it.
		if ( sz <= 0.0 )
			continue;
		{
			const Float v = std::abs(at.x_ + sz);
			if ( (max_sz < 0.0) || (max_sz < v) )
				max_sz = v;
		}
		{
			const Float v = std::abs(at.x_ - sz);
			if ( (max_sz < 0.0) || (max_sz < v) )
				max_sz = v;
		}
		{
			const Float v = std::abs(at.y_ + sz);
			if ( (max_sz < 0.0) || (max_sz < v) )
				max_sz = v;
		}
		{
			const Float v = std::abs(at.y_ - sz);
			if ( (max_sz < 0.0) || (max_sz < v) )
				max_sz = v;
		}
		{
			const Float v = std::abs(at.z_ + sz);
			if ( (max_sz < 0.0) || (max_sz < v) )
				max_sz = v;
		}
		{
			const Float v = std::abs(at.z_ - sz);
			if ( (max_sz < 0.0) || (max_sz < v) )
				max_sz = v;
		}
	}

	if (total_max_size > 0.0)
	{
		if (max_sz < 0.0)
			max_sz = total_max_size;
		else if ( (max_sz > 0.0) && (total_max_size < max_sz) )
			max_sz = total_max_size;
	}
	const Float ratio = std::pow( 2.0, static_cast<Float>(max_depth) );
	step      = max_sz / ratio;
}

void CubeTree::subdivide( Float total_max_size )
{
	// Cleanup everything.
	nodes.clear();
	sources.clear();

	// Search for all occurences of OctreeMeshGd instances.
	fill_source_references();

	const int sources_qty = sources.size();
	if ( sources_qty < 1 )
		return;

	compute_levels( total_max_size );
	int at = 1;
	for ( int i=0; i<(max_depth-1); i++ )
		at *= 2;

	CubeTreeNode root;

	root.corner = VectorInt( -at, -at, -at );
	root.size  = at * 2;
	root.init( this );
	root.source_inds.clear();
	for ( int i=0; i<sources_qty; i++ )
		root.source_inds.push_back( i );

	insert_node( root );

	root.subdivide( this );
	update_node( root );
}

bool CubeTree::parent( const CubeTreeNode & node, CubeTreeNode * & parent )
{
	if ( node.parent_abs_index < 0 )
	{
		parent = nullptr;
		return false;
	}

	parent = &( nodes[ node.parent_abs_index ] );
	return true;
}

int  CubeTree::insert_node( CubeTreeNode & node )
{
	nodes.push_back( node );
	const int ind = static_cast<int>(nodes.size()) - 1;
	CubeTreeNode & n = nodes[ind];
	n.abs_index = ind;

	node = n;
	return ind;
}

void CubeTree::update_node( const CubeTreeNode & node )
{
	nodes[ node.abs_index ] = node;
}

Vector3d CubeTree::at( const VectorInt & at_i ) const
{
	Vector3d ret;
	ret.x_ = static_cast<Float>(at_i.x) * step;
	ret.y_ = static_cast<Float>(at_i.y) * step;
	ret.z_ = static_cast<Float>(at_i.z) * step;
	return ret;
}






}







