
#include "physics_item.h"
#include "physics_frame.h"



namespace Ign
{

void PhysicsItem::RegisterComponent( Context * context )
{
    context->RegisterFactory<PhysicsItem>();
    URHO3D_COPY_BASE_ATTRIBUTES( RefFrame );
}

PhysicsItem::PhysicsItem( Context * context )
    : RefFrame( context )
{
}

PhysicsItem::~PhysicsItem()
{
}

void PhysicsItem::DrawDebugGeometry( DebugRenderer * debug, bool depthTest )
{
    RefFrame::DrawDebugGeometry( debug, depthTest );
    if ( rigid_body_ )
        rigid_body_->DrawDebugGeometry( debug, depthTest );
    if ( collision_shape_ )
        collision_shape_->DrawDebugGeometry( debug, depthTest );
}

void PhysicsItem::updateStateFromRigidBody()
{
    if ( !rigid_body_ )
        return;
    // Assign state variables from rigid body state.
    const Vector3    r = rigid_body_->GetPosition();
    const Quaternion q = rigid_body_->GetRotation();
    const Vector3    v = rigid_body_->GetLinearVelocity();
    const Vector3    w = rigid_body_->GetAngularVelocity();
    st_.r = Vector3d( r.x_, r.y_, r.z_ );
    st_.q = Quaterniond( q.w_, q.x_, q.y_, q.z_ );
    st_.v = Vector3d( v.x_, v.y_, v.z_ );
    st_.w = Vector3d( w.x_, w.y_, w.z_ );
    
    MarkNetworkUpdate();
}

RigidBody2* PhysicsItem::rigidBody() const
{
    return rigid_body_;
}

void PhysicsItem::enteredRefFrame( RefFrame * refFrame )
{
    Environment * env_ = this->env();
    if ( !env_ )
        return;

    const bool isServer = env_->IsServer();
    if ( !isServer )
        return;


    // Check if it is a physics ref frame.
    // And if yes create physics content.
    if ( !refFrame )
        return;
    PhysicsFrame * pf = refFrame->Cast<PhysicsFrame>();
    if ( !pf )
        return;
    Node * node = pf->physicsNode();
    if ( !node )
        return;
    const String n = String( this->name() ) + String( " physics node" );
    physics_node_    = node->CreateChild( n, LOCAL );
    rigid_body_      = physics_node_->CreateComponent<RigidBody2>( LOCAL );
    collision_shape_ = physics_node_->CreateComponent<CollisionShape2>( LOCAL );
    
    setupPhysicsContent( rigid_body_, collision_shape_ );

    // Assign state to rigid body.
    rigid_body_->SetPosition( Vector3( st_.r.x_, st_.r.y_, st_.r.z_ ) );
    rigid_body_->SetRotation( Quaternion( st_.q.w_, st_.q.x_, st_.q.y_, st_.q.z_ ) );
    rigid_body_->SetLinearVelocity( Vector3( st_.v.x_, st_.v.y_, st_.v.z_ ) );
    rigid_body_->SetAngularVelocity( Vector3( st_.w.x_, st_.w.y_, st_.v.z_ ) );
}

void PhysicsItem::leftRefFrame( RefFrame * refFrame )
{
    // Need to destroy physics content.
    if ( physics_node_ )
        physics_node_->Remove();
}

void PhysicsItem::childEntered( RefFrame * refFrame )
{
    Scene * scene = GetScene();
    if ( !scene )
        return;

    const bool controlledByUser = getUserControlled();
    if ( !controlledByUser )
        return;

    Environment * env_ = this->env();
    if ( !env_ )
        return;

    const bool isServer = env_->IsServer();
    if ( !isServer )
        return;

    // Check if parent is physics frame.
    if ( parent_ )
    {
        PhysicsFrame * pf = parent_->Cast<PhysicsFrame>();
        if ( pf )
            return;
    }

    // For simplicity just create new physics frame and place this object into it.
    // Physics frame will take all the logic load concerning if it needs to merge
    // with anything, etc.
    PhysicsFrame * pf = scene->CreateComponent<PhysicsFrame>();
    pf->setParent( this->parent_ );
    pf->setState( this->state() );
    this->setParent( pf );
}

void PhysicsItem::childLeft( RefFrame * refFrame )
{

}

void PhysicsItem::OnSceneSet( Scene * scene )
{
    if ( !scene )
    {
        if ( visual_node_ )
            visual_node_->Remove();
        return;
    }
    
    // Create visuals to the "visual_node_"
    const String n = this->name() + " visual node";
    visual_node_ = scene->CreateChild( n );
    assignRefFrame( visual_node_ );
    createVisualContent( visual_node_ );
}


void PhysicsItem::createVisualContent( Node * n )
{
}

void PhysicsItem::setupPhysicsContent( RigidBody2 * rb, CollisionShape2 * cs )
{
}


}





