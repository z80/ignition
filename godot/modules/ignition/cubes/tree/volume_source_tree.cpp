
#include "volume_source_tree.h"

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
    const ret = tree.get_max_items_per_node();
    return ret;
}

Float VolumeSourceTree::value( const Vector3d & at ) const
{
}

void VolumeSourceTree::clear()
{
    sources.clear();
    tree.clear();
}

void VolumeSourceTree::add_source( Src & source )
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











