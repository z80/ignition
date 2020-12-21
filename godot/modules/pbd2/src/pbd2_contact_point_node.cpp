
#include "pbd2_contact_point_node.h"
#include "pbd2_rigid_body_node.h"
#include "pbd2_rigid_body.h"

namespace Pbd
{

static PbdRigidBodyNode * get_parent_body( const Node * n )
{
    Node * p = n->get_parent();
    PbdRigidBodyNode * rb = Node::cast_to<PbdRigidBodyNode>( p );
    return rb;
}

PbdContactPointNode::PbdContactPointNode()
    : Spatial()
{
}

PbdContactPointNode::~PbdContactPointNode()
{
}

String PbdContactPointNode::get_configuration_warning() const
{
    String pre = Spatial::get_configuration_warning();

    PbdRigidBodyNode * body = get_parent_body( this );
    if ( body == nullptr )
    {
        const String post = "\nWarning: should be a child of PbdRigidBodyNode";
        pre += post;
    }

    return pre;
}

void PbdContactPointNode::_notification( int p_what )
{
    Spatial::_notification( p_what );

    switch ( p_what )
    {
        case NOTIFICATION_READY:
            break;

        default:
            break;
    }
}

void PbdContactPointNode::_bind_methods()
{
}


}


