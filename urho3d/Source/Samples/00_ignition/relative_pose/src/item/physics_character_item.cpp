
#include "physics_character_item.h"

namespace Ign
{

void PhysicsCharacterItem::RegisterComponent( Context * context )
{
    context->RegisterFactory<PhysicsCharacterItem>();
    URHO3D_COPY_BASE_ATTRIBUTES( PhysicsItem );

    URHO3D_ATTRIBUTE( "Azimuth", double, azimuth_, 0.0, AM_DEFAULT );
}

PhysicsCharacterItem::PhysicsCharacterItem( Context * context )
    : PhysicsItem( context )
{
    azimuth_ = 0.0;
}

PhysicsCharacterItem::~PhysicsCharacterItem()
{
}

void PhysicsCharacterItem::SetAzimuth( Float az )
{
    azimuth_ = az;
}

Float PhysicsCharacterItem::azimuth() const
{
    return azimuth_;
}

void PhysicsCharacterItem::setupPhysicsContent( RigidBody2 * rb, CollisionShape2 * cs )
{
    PhysicsItem::setupPhysicsContent( rb, cs );
    // All rotational degrees of freedom are disabled.
    rb->SetAngularFactor( Vector3::ZERO );
}

void PhysicsCharacterItem::physicsUpdate( RigidBody2 * rb )
{

}

void PhysicsCharacterItem::orientRigidBody( RigidBody * rb )
{

}


}





