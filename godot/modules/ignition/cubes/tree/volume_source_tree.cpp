
#include "volume_source_tree.h"

#include <algorithm>

namespace Ign
{

VolumeSourceTree::VolumeSourceTree()
    : VolumeSource()
{
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
        return -1.0;
    VolumeSource * vs = reinterpret_cast<VolumeSource *>( objs[0] );
    Float dist = vs->value_global( at );
    for ( int i=1; i<qty; i++ )
    {
        vs = reinterpret_cast<VolumeSource *>( objs[i] );
        const bool is_inverted = vs->get_inverted();
        const Float dist_i = vs->value_global( at );
        if ( is_inverted )
            dist = std::max( dist, -dist_i );
        else
            dist = std::min( dist, dist_i );
    }

    return dist;
}

void VolumeSourceTree::clear()
{
    sources.clear();
    tree.clear();
}

void VolumeSourceTree::add_source( const Src & source )
{
    VolumeSource * src = source.ptr()->source;
    sources.push_back( source );
    tree.add_source( src );
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











