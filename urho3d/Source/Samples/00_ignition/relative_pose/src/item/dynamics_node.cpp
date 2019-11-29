
#include "dynamics_node.h"
#include "settings.h"

namespace Ign
{


DynamicsNode::DynamicsNode( Context * context )
    : RefFrame( context )
{

}

DynamicsNode::~DynamicsNode()
{

}

void DynamicsNode::dynamicsStep( float sec_dt )
{
    if ( !physicsWorld_ )
        return;
    physicsWorld_->Update( sec_dt );
}

void DynamicsNode::childEntered( RefFrame * refFrame )
{

}

void DynamicsNode::childLeft( RefFrame * refFrame )
{

}

void DynamicsNode::OnSceneSet( Scene * scene )
{
    if ( !scene )
        return;

    env_ = scene->GetComponent<Environment>();
    if ( !env_ )
        return;

    const bool isServer = env_->IsServer();
    if ( !isServer )
        return;

    const String name( "Dynamics node" );
    Node * n = scene->CreateChild( name, LOCAL );
    node_         = SharedPtr<Node>( n );
    physicsWorld_ = n->CreateComponent<PhysicsWorld2>( LOCAL );
}

void DynamicsNode::checkInnerObjects()
{
    // Select user controlled objects.
    static Vector<SharedPtr<RefFrame> > userControlled;
    userControlled.Clear();
    const unsigned qty = children_.Size();
    for ( unsigned i=0 ;i<qty; i++ )
    {
        SharedPtr<RefFrame> o = children_[i];
        if ( o->getUserControlled() )
            userControlled.Push( o );
    }

    if ( userControlled.Empty() )
        return;
}

void DynamicsNode::checkOuterObjects()
{
    // At the moment don't know how to deal with it.
}

void DynamicsNode::checkIfWorthToExist()
{
    // If there are no user objects remove this object.
}

void DynamicsNode::checkIfTeleport( const Vector<SharedPtr<RefFrame>> & objs )
{
    SharedPtr<RefFrame> o = objs.At(0);
    // Follow the first one.
    const Float teleportDist = Settings::teleportDistance();
    const Float d = o->distance();
    if ( d > teleportDist )
    {
        const Vector3d & r = o->relR();
        teleport( parent_, r );
    }
}



}


