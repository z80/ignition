
#include "volume_source_tree_gd.h"

namespace Ign
{

void VolumeSourceTreeGd::_bind_methods()
{
    ClassDB::bind_method( D_METHOD("set_max_depth", "depth"), &VolumeSourceTreeGd::set_max_depth );
    ClassDB::bind_method( D_METHOD("get_max_depth"),          &VolumeSourceTreeGd::get_max_depth, Variant::INT );

    ClassDB::bind_method( D_METHOD("set_max_items_per_node", "qty"), &VolumeSourceTreeGd::set_max_items_per_node );
    ClassDB::bind_method( D_METHOD("get_max_items_per_node"),        &VolumeSourceTreeGd::get_max_items_per_node, Variant::INT );

    ClassDB::bind_method( D_METHOD("clear"),                 &VolumeSourceTreeGd::clear );
    ClassDB::bind_method( D_METHOD("add_source", "source"),  &VolumeSourceTreeGd::add_source );

    ClassDB::bind_method( D_METHOD("subdivide", "total_max_size"), &VolumeSourceTreeGd::subdivide );
    ClassDB::bind_method( D_METHOD("nodes_qty"),                   &VolumeSourceTreeGd::nodes_qty, Variant::INT );

    ADD_PROPERTY( PropertyInfo( Variant::INT, "max_depth" ),          "set_max_depth",          "get_max_depth" );
    ADD_PROPERTY( PropertyInfo( Variant::INT, "max_items_per_node" ), "set_max_items_per_node", "get_max_items_per_node" );
}

VolumeSourceTreeGd::VolumeSourceTreeGd()
    : VolumeSourceGd()
{
    source = &tree;
    object = &tree;
}

VolumeSourceTreeGd::~VolumeSourceTreeGd()
{
}

void VolumeSourceTreeGd::set_max_depth( int d )
{
    tree.set_max_depth( d );
}

int  VolumeSourceTreeGd::get_max_depth() const
{
    const int ret = tree.get_max_depth();
    return ret;
}

void VolumeSourceTreeGd::set_max_items_per_node( int qty )
{
    tree.set_max_items_per_node( qty );
}

int  VolumeSourceTreeGd::get_max_items_per_node() const
{
    const int ret = tree.get_max_items_per_node();
    return ret;
}

real_t VolumeSourceTreeGd::value( const Vector3 & at )
{
    const Vector3d at_d( at.x, at.y, at.z );
    const real_t ret = tree.value( at_d );
    return ret;
}

void VolumeSourceTreeGd::clear()
{
    tree.clear();
}

void VolumeSourceTreeGd::add_source( const Ref<VolumeSourceGd> & source )
{
    tree.add_source( source );
}

void VolumeSourceTreeGd::subdivide( real_t total_max_size )
{
    tree.subdivide( total_max_size );
}

int VolumeSourceTreeGd::nodes_qty() const
{
    const int ret = tree.nodes_qty();
    return ret;
}






















}














