
#include "pbd2_contact_point_node.h"
#include "pbd2_rigid_body_node.h"
#include "pbd2_rigid_body.h"

namespace Pbd
{

static RigidBodyNode * get_parent_body( Node * n )
{
    Node * p = n->get_parent();
    RigidBodyNode * rb = Node::cast_to<RigidBodyNode>( p );
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

    RigidBody * body = get_parent_body();
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


