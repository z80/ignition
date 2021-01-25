
#include "pbd2_collision_object_node.h"
#include "pbd2_rigid_body_node.h"
#include "pbd2_rigid_body.h"

#include "core/engine.h"


namespace Pbd
{

static void apply_pose( PbdCollisionObjectNode * con )
{
    if ( (con == nullptr) || (con->collision_object == nullptr) )
        return;
    const Pose p = con->collision_object->pose;
    const Vector3d & r_ = p.r;
    const Quaterniond & q_ = p.q;
    const Vector3 r( r_.x_, r_.y_, r_.z_ );
    const Quat q( q_.x_, q_.y_, q_.z_, q_.w_ );

    Transform t = con->get_transform();
    t.set_origin( r );
    t.set_basis( q );
    con->set_transform( t );
}

static void enter_tree( PbdCollisionObjectNode * con )
{
    if ( (con == nullptr) || (con->collision_object == nullptr) )
        return;
    Node * n = con->get_parent();
    PbdRigidBodyNode * rbn = Node::cast_to<PbdRigidBodyNode>( n );
    if ( rbn == nullptr )
        return;
    rbn->rigid_body.add_collision( con->collision_object );
}

static void exit_tree( PbdCollisionObjectNode * con )
{
    if ( (con == nullptr) || (con->collision_object == nullptr) )
        return;
    Node * n = con->get_parent();
    PbdRigidBodyNode * rbn = Node::cast_to<PbdRigidBodyNode>( n );
    if ( rbn == nullptr )
        return;

    rbn->rigid_body.remove_collision( con->collision_object );
}

static void set_transform_con( PbdCollisionObjectNode * con, const Transform & t )
{
    if ( (con == nullptr) || (con->collision_object == nullptr) )
        return;
    const Vector3 & o = t.origin;
    const Quat q = t.basis.get_quat();
    con->collision_object->pose.r = Vector3d( o.x, o.y, o.z );
    con->collision_object->pose.q = Quaterniond( q.w, q.x, q.y, q.z );
}

void PbdCollisionObjectNode::_bind_methods()
{
}

void PbdCollisionObjectNode::_notification( int p_what )
{
    switch ( p_what )
    {
        case NOTIFICATION_PROCESS:
            if ( !Engine::get_singleton()->is_editor_hint() )
                apply_pose( this );        
            break;

        case NOTIFICATION_ENTER_TREE:
            enter_tree( this );
            break;

        case NOTIFICATION_EXIT_TREE:
            exit_tree( this );

        case NOTIFICATION_LOCAL_TRANSFORM_CHANGED:
			if ( Engine::get_singleton()->is_editor_hint() )
				set_transform_con( this, get_transform() );
            break;

        default:
            break;
    }
}



PbdCollisionObjectNode::PbdCollisionObjectNode()
    : Spatial()
{
	set_process( true );
	set_notify_local_transform( true );
}

PbdCollisionObjectNode::~PbdCollisionObjectNode()
{
}





}










