
#include "physics_item.h"
#include "physics_frame.h"



namespace Ign
{

void PhysicsItem::RegisterObject( Context * context )
{
    context->RegisterFactory<RefFrame>();
    URHO3D_COPY_BASE_ATTRIBUTES( RefFrame );
}

PhysicsItem::PhysicsItem( Context * context )
    : RefFrame( context )
{
}

PhysicsItem::~PhysicsItem()
{
}

void PhysicsItem::enteredRefFrame( RefFrame * refFrame )
{
    // Check if it is a physics ref frame.
    // And if yes create physics content.
    if ( !refFrame )
        return;
    PhysicsFrame * pf = refFrame->Cast<PhysicsFrame>();
    if ( !pf )
        return;
    Node * parentNode = pf->GetNode();
    if ( !parentNode )
        return;
    const String n = String( this->name() ) + String( " physics node" );
    physics_node_    = parentNode->CreateChild( n );
    rigid_body_      = physics_node_->CreateComponent<RigidBody2>();
    collision_shape_ = physics_node_->CreateComponent<CollisionShape2>();
    setupPhysicsContent( rigid_body_, collision_shape_ );
}

void PhysicsItem::leftRefFrame( RefFrame * refFrame )
{
    // Need to destroy physics content.
    physics_node_->Remove();
}

void PhysicsItem::userControlledChanged( bool newUserControlled )
{
    Scene * scene = GetScene();
    if ( !scene )
        return;

    env_ = scene->GetComponent<Environment>();
    if ( !env_ )
        return;

    const bool isServer = env_->IsServer();
    if ( !isServer )
        return;

    // Check if parent is physics frame.
    if ( parent_ )
    {
        PhysicsFrame * pf = parent->->Cast<PhysicsFrame>();
        if ( pf )
            return;
    }

    // For simplicity just create new physics frame and place this object into it.
    // Physics frame will take all the logic load concerning if it needs to merge 
    // with anything, etc.
    PhysicsFrame * pf = scene_->CreateComponent<PhysicsFrame>();
    pf->setParent( this->parent_ );
    pf->setState( this->state() );
    this->setParent( pf );
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
    createVisualContent( visual_node_ );
}


void PhysicsItem::createVisualContent( Node * n )
{
}

void PhysicsItem::setupPhysicsContent( RigidBody2 * rb, CollisionShape2 * cs )
{
}


}





