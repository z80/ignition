
#include "physics_character_item.h"
#include "orbiting_frame.h"

namespace Ign
{

// Recursively searches for orbiting frame in order to compute
// direction towards its origin.
static OrbitingFrame * orbitingFrame( RefFrame * rf );

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
    orientRigidBody( rb );
}

void PhysicsCharacterItem::orientRigidBody( RigidBody2 * rb )
{
    // It doesn't work yet.

    RefFrame * rf = parent();
    OrbitingFrame * of = orbitingFrame( rf );
    if ( !of )
        return;
    const Quaterniond localQ = relQ();
    const Vector3d actualG = localQ * Vector3d( 0.0, -1.0, 0.0 );
    State rs;
    of->relativeState( this, rs, true );
    const Vector3d localWantedG = -rs.r.Normalized();
    const Vector3d wantedG = localQ * localWantedG;
    Quaterniond q;
    q.FromRotationTo( actualG, wantedG );
    q = q * localQ;

    rb->SetRotationd( q );
    setQ( q );
}

static OrbitingFrame * orbitingFrame( RefFrame * rf )
{
    if ( !rf )
        return nullptr;
    OrbitingFrame * of = rf->Cast<OrbitingFrame>();
    if ( of )
        return of;
    RefFrame * p = rf->parent();
    of = orbitingFrame( p );
    return of;
}


}





