
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
}

void PhysicsItem::leftRefFrame( RefFrame * refFrame )
{
}

void PhysicsItem::parentTeleported()
{
}

void PhysicsItem::childTeleported( RefFrame * refFrame )
{
}

void PhysicsItem::userControlledChanged( bool newUserControlled )
{
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
}

}





