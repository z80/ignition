
#include "pbd2_narrow_tree_node.h"
#include "pbd2_rigid_body_node.h"
#include "pbd2_simulation_node.h"

#include "scene/3d/mesh_instance.h"


namespace Pbd
{







void PbdCollisionSdfMeshTreeNode::_bind_methods()
{
    ClassDB::bind_method( D_METHOD( "set_min_depth", "depth" ), &PbdCollisionSdfMeshTreeNode::set_min_depth );
    ClassDB::bind_method( D_METHOD( "get_min_depth" ), &PbdCollisionSdfMeshTreeNode::get_min_depth, Variant::INT );
    ClassDB::bind_method( D_METHOD( "set_max_depth", "depth" ), &PbdCollisionSdfMeshTreeNode::set_max_depth );
    ClassDB::bind_method( D_METHOD( "get_max_depth" ), &PbdCollisionSdfMeshTreeNode::get_max_depth, Variant::INT );
    ClassDB::bind_method( D_METHOD( "set_max_error", "err" ), &PbdCollisionSdfMeshTreeNode::set_max_error );
    ClassDB::bind_method( D_METHOD( "get_max_error" ), &PbdCollisionSdfMeshTreeNode::get_max_error, Variant::REAL );
    ClassDB::bind_method( D_METHOD( "set_min_points", "qty" ), &PbdCollisionSdfMeshTreeNode::set_min_points );
    ClassDB::bind_method( D_METHOD( "get_min_points" ), &PbdCollisionSdfMeshTreeNode::get_min_points, Variant::INT );

    ClassDB::bind_method( D_METHOD( "subdivide" ), &PbdCollisionSdfMeshTreeNode::subdivide );

    ClassDB::bind_method( D_METHOD( "lines_nodes_sdf" ), &PbdCollisionSdfMeshTreeNode::lines_nodes_sdf, Variant::POOL_VECTOR3_ARRAY );
    ClassDB::bind_method( D_METHOD( "lines_nodes_pts" ), &PbdCollisionSdfMeshTreeNode::lines_nodes_pts, Variant::POOL_VECTOR3_ARRAY );
    ClassDB::bind_method( D_METHOD( "lines_pts" ),       &PbdCollisionSdfMeshTreeNode::lines_pts, Variant::POOL_VECTOR3_ARRAY );

    ADD_PROPERTY( PropertyInfo( Variant::INT,  "min_depth" ), "set_min_depth", "get_min_depth" );
    ADD_PROPERTY( PropertyInfo( Variant::INT,  "max_depth" ), "set_max_depth", "get_max_depth" );
    ADD_PROPERTY( PropertyInfo( Variant::REAL, "max_error" ), "set_max_error", "get_max_error" );
    ADD_PROPERTY( PropertyInfo( Variant::INT,  "min_points" ), "set_min_points", "get_min_points" );
}

void PbdCollisionSdfMeshTreeNode::_notification( int p_what )
{
    switch ( p_what )
    {
        case NOTIFICATION_READY:
            subdivide();
            break;

        default:
            break;
    }
}


PbdCollisionSdfMeshTreeNode::PbdCollisionSdfMeshTreeNode()
    : PbdCollisionObjectNode()
{
    collision_object = &tree;
}

PbdCollisionSdfMeshTreeNode::~PbdCollisionSdfMeshTreeNode()
{
}

void PbdCollisionSdfMeshTreeNode::set_min_depth( int d )
{
    tree.set_min_depth( d );
}

int  PbdCollisionSdfMeshTreeNode::get_min_depth() const
{
    return tree.min_depth();
}

void PbdCollisionSdfMeshTreeNode::set_max_depth( int d )
{
    tree.set_max_depth( d );
}

int  PbdCollisionSdfMeshTreeNode::get_max_depth() const
{
    return tree.max_depth();
}

void PbdCollisionSdfMeshTreeNode::set_max_error( real_t err )
{
    tree.set_max_error( err );
}

real_t PbdCollisionSdfMeshTreeNode::get_max_error() const
{
    return tree.max_error();
}

void PbdCollisionSdfMeshTreeNode::set_min_points( int qty )
{
    return tree.set_min_points( qty );
}

int  PbdCollisionSdfMeshTreeNode::get_min_points() const
{
    return tree.min_points();
}

void PbdCollisionSdfMeshTreeNode::subdivide()
{
    parse_children();
    tree.subdivide();
}

PoolVector3Array PbdCollisionSdfMeshTreeNode::lines_nodes_sdf()
{
    PoolVector3Array ret = tree.lines_sdf_nodes();
    return ret;
}

PoolVector3Array PbdCollisionSdfMeshTreeNode::lines_nodes_pts()
{
    PoolVector3Array ret = tree.lines_pts_nodes();
    return ret;
}

PoolVector3Array PbdCollisionSdfMeshTreeNode::lines_pts()
{
    PoolVector3Array ret = tree.lines_surface_pts();
    return ret;
}





void PbdCollisionSdfMeshTreeNode::parse_children()
{
    tree.clear();

    const int children_qty = get_child_count();
    for ( int k=0; k<children_qty; k++ )
    {
        Node * n = get_child( k );
        MeshInstance * mi = Node::cast_to<MeshInstance>( n );
        if ( mi == nullptr )
            continue;
        Ref<Mesh> mesh = mi->get_mesh();
        const Transform t = mi->get_transform();
        tree.append( t, mesh );
    }
}




}











