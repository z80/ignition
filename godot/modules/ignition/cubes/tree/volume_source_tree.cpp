
#include "volume_source_tree.h"

#include <algorithm>

namespace Ign
{

VolumeSourceTree::VolumeSourceTree()
    : VolumeSource()
{
	_max_node_size = -1.0;
	_min_node_size = -1.0;
}

VolumeSourceTree::~VolumeSourceTree()
{
}

void VolumeSourceTree::set_max_depth( int d )
{
    tree.set_max_depth( d );
}

int  VolumeSourceTree::get_max_depth() const
{
    const int ret = tree.get_max_depth();
    return ret;
}

void VolumeSourceTree::set_max_items_per_node( int qty )
{
    tree.set_max_items_per_node( qty );
}

int  VolumeSourceTree::get_max_items_per_node() const
{
    const int ret = tree.get_max_items_per_node();
    return ret;
}

Float VolumeSourceTree::value( const Vector3d & at )
{
    // Here "at" is in local ref. frame with respect to 
    // this tree object.
    const std::vector<MarchingVolumeObject *> & objs = tree.pick_objects( at );
    const int qty = objs.size();
    if (qty < 1)
	{
		const Float dist = at.Length();
		return dist;
	}

	bool initiated = false;
	Float dist;
	int ind = 0;
	for ( ; ind<qty; ind++ )
	{
		VolumeSource * vs = reinterpret_cast<VolumeSource *>( objs[ind] );
		const bool is_inverted = vs->get_inverted();
		if ( !is_inverted )
		{
			initiated = true;
			dist = vs->value_global( at );
			break;
		}
	}

	if ( !initiated )
	{
		const Float dist = at.Length();
		return dist;
	}

	ind += 1;
    for ( int i=ind; i<qty; i++ )
    {
		VolumeSource * vs = reinterpret_cast<VolumeSource *>( objs[i] );
        const bool is_inverted = vs->get_inverted();
        const Float dist_i = vs->value_global( at );
        if ( is_inverted )
            dist = std::max( dist, -dist_i );
        else
            dist = std::min( dist, dist_i );
    }

    return dist;
}

Float VolumeSourceTree::max_node_size() const
{
	return _max_node_size;
}

Float VolumeSourceTree::min_node_size() const
{
	return _min_node_size;
}

Float VolumeSourceTree::max_node_size_local( const Vector3d & at )
{
	const std::vector<MarchingVolumeObject *> & objs = tree.pick_objects( at );
	const int qty = objs.size();
	Float max_sz = _max_node_size;
	if (qty < 1)
		return max_sz;

	max_sz = -1.0;
	for ( int i=0; i<qty; i++ )
	{
		VolumeSource * vs = reinterpret_cast<VolumeSource *>( objs[i] );

		const Float dist = (at - vs->se3.r_).Length();
		const Float r = vs->get_bounding_radius();
		if ( dist > r )
			continue;

		const Float sz = vs->max_node_size_at( at );
		if (sz > max_sz)
			max_sz = sz;
	}

	return max_sz;
}

Float VolumeSourceTree::min_node_size_local( const Vector3d & at )
{
	const std::vector<MarchingVolumeObject *> & objs = tree.pick_objects( at );
	const int qty = objs.size();
	Float min_sz = _min_node_size;
	if (qty < 1)
		return min_sz;

	min_sz = _max_node_size;
	for ( int i=0; i<qty; i++ )
	{
		VolumeSource * vs = reinterpret_cast<VolumeSource *>( objs[i] );

		const Float dist = (at - vs->se3.r_).Length();
		const Float r = vs->get_bounding_radius();
		if ( dist > r )
			continue;

		const Float sz = vs->max_node_size_at( at );
		if ( sz < min_sz )
			min_sz = sz;
	}

	return min_sz;
}



int VolumeSourceTree::material( const Vector3d & at, int * priority )
{
	// Here "at" is in local ref. frame with respect to 
	// this tree object.
	const std::vector<MarchingVolumeObject *> & objs = tree.pick_objects( at );
	const int qty = objs.size();
	if (qty < 1)
	{
		if ( priority != nullptr )
			*priority = -1;
		return -1;
	}

	bool initiated = false;
	int start_ind = 0;
	int material_ind = -1;
	int priority_ind = -1;
	for ( start_ind=0; start_ind<qty; start_ind++ )
	{
		VolumeSource * vs = reinterpret_cast<VolumeSource *>( objs[start_ind] );
		const Float value = vs->value_global( at );
		if ( value > 0.0 )
			continue;
		int priority_ind_i;
		const int material_ind_i = vs->material_global( at, &priority_ind_i );
		material_ind = material_ind_i;
		if ( material_ind_i >= 0 )
		{
			material_ind = material_ind_i;
			priority_ind = priority_ind_i;
			break;
		}
	}

	// If not initialized, return failure.
	if ( material_ind < 0 )
	{
		if ( priority != nullptr )
			*priority = -1;
		return -1;
	}

	start_ind += 1;
	for ( int i=start_ind; i<qty; i++ )
	{
		VolumeSource * vs = reinterpret_cast<VolumeSource *>( objs[i] );
		const Float value = vs->value_global( at );
		if ( value > 0.0 )
			continue;
		int priority_ind_i;
		const int material_ind_i = vs->material_global( at, &priority_ind_i );
		if ( (material_ind_i >= 0) && ( priority_ind_i >= priority_ind ) )
		{
			material_ind = material_ind_i;
			priority_ind = priority_ind_i;
			break;
		}
	}

	if ( priority != nullptr )
		*priority = priority_ind;
	return material_ind;
}



void VolumeSourceTree::clear()
{
	_max_node_size = -1.0;
	_min_node_size = -1.0;

    sources.clear();
    tree.clear();
}

void VolumeSourceTree::add_source( const Src & source )
{
    VolumeSource * src = source.ptr()->source;
    sources.push_back( source );
    tree.add_source( src );

	// Update min and max node sizes.
	const Float min_sz = src->min_node_size();
	const Float max_sz = src->max_node_size();
	if ( ( _min_node_size < 0.0 ) || ( min_sz < _min_node_size ) )
		_min_node_size = min_sz;
	if ( (_max_node_size < 0.0) || (max_sz > _max_node_size) )
		_max_node_size = max_sz;
}

void VolumeSourceTree::subdivide( Float total_max_size )
{
    tree.subdivide( total_max_size );
}

int VolumeSourceTree::nodes_qty() const
{
    const int ret = tree.nodes.size();
    return ret;
}









    
}











