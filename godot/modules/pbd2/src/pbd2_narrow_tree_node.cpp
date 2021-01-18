
#include "pbd2_narrow_tree_node.h"
#include "pbd2_rigid_body_node.h"
#include "pbd2_simulation_node.h"

#include "scene/3d/mesh_instance.h"


namespace Pbd
{

static void enter_tree( PbdNarrowTreeNode * rbn )
{
    Node * n = rbn->get_parent();
    PbdRigidBodyNode * rb = Node::cast_to<PbdRigidBodyNode>( n );
    if ( rb == nullptr )
        return;
    rbn->rigid_body = &(rb->rigid_body);
    n = n->get_parent();
    PbdSimulationNode * sn = Node::cast_to<PbdSimulationNode>( n );
    if ( sn == nullptr )
        return;
    Simulation & s = sn->simulation;
    //s.bodies.push_back( &(rbn->rigid_body) );
}

static void exit_tree( PbdNarrowTreeNode * rbn )
{
    Node * n = rbn->get_parent();
    PbdRigidBodyNode * rb = Node::cast_to<PbdRigidBodyNode>( n );
    if ( rb == nullptr )
        return;
    n = n->get_parent();
    PbdSimulationNode * sn = Node::cast_to<PbdSimulationNode>( n );
    if ( sn == nullptr )
        return;
    Simulation & s = sn->simulation;
    //s.bodies.erase( &(rbn->rigid_body) );
}







void PbdNarrowTreeNode::_bind_methods()
{
    ClassDB::bind_method( D_METHOD( "set_min_depth", "d" ), &PbdNarrowTreeNode::set_min_depth );
    ClassDB::bind_method( D_METHOD( "get_min_depth" ), &PbdNarrowTreeNode::get_min_depth, Variant::INT );
    ClassDB::bind_method( D_METHOD( "set_max_depth" "d" ), &PbdNarrowTreeNode::set_max_depth );
    ClassDB::bind_method( D_METHOD( "get_max_depth" ), &PbdNarrowTreeNode::get_max_depth, Variant::INT );
    ClassDB::bind_method( D_METHOD( "set_max_error", "err" ), &PbdNarrowTreeNode::set_max_error );
    ClassDB::bind_method( D_METHOD( "get_max_error" ), &PbdNarrowTreeNode::get_max_error, Variant::REAL );
    ClassDB::bind_method( D_METHOD( "set_min_points", "qty" ), &PbdNarrowTreeNode::set_min_points );
    ClassDB::bind_method( D_METHOD( "get_min_points" ), &PbdNarrowTreeNode::get_min_points, Variant::INT );

    ClassDB::bind_method( D_METHOD( "subdivide" ), &PbdNarrowTreeNode::subdivide );

    ADD_PROPERTY( PropertyInfo( Variant::INT,  "min_depth" ), "set_min_depth", "get_min_depth" );
    ADD_PROPERTY( PropertyInfo( Variant::INT,  "max_depth" ), "set_max_depth", "get_max_depth" );
    ADD_PROPERTY( PropertyInfo( Variant::REAL, "max_error" ), "set_max_error", "get_max_error" );
    ADD_PROPERTY( PropertyInfo( Variant::INT,  "mmin_points" ), "set_min_points", "get_min_points" );
}

void PbdNarrowTreeNode::_notification( int p_what )
{
    switch ( p_what )
    {
        case NOTIFICATION_READY:
			parse_children();
            break;

        case NOTIFICATION_ENTER_TREE:
            enter_tree( this );
            break;

        case NOTIFICATION_EXIT_TREE:
            exit_tree( this );

        default:
            break;
    }
}


PbdNarrowTreeNode::PbdNarrowTreeNode()
{
    rigid_body = nullptr;
}

PbdNarrowTreeNode::~PbdNarrowTreeNode()
{
}

void PbdNarrowTreeNode::set_min_depth( int d )
{
    tree.set_min_depth( d );
}

int  PbdNarrowTreeNode::get_min_depth() const
{
    return tree.min_depth();
}

void PbdNarrowTreeNode::set_max_depth( int d )
{
    tree.set_max_depth( d );
}

int  PbdNarrowTreeNode::get_max_depth() const
{
    return tree.max_depth();
}

void PbdNarrowTreeNode::set_max_error( real_t err )
{
    tree.set_max_error( err );
}

real_t PbdNarrowTreeNode::get_max_error() const
{
    return tree.max_error();
}

void PbdNarrowTreeNode::set_min_points( int qty )
{
    return tree.set_min_points( qty );
}

int  PbdNarrowTreeNode::get_min_points() const
{
    return tree.min_points();
}

void PbdNarrowTreeNode::subdivide()
{
    tree.subdivide();
}






void PbdNarrowTreeNode::parse_children()
{

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











