
#include "physics_character_item.h"
#include "orbiting_frame.h"

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
    geocentric_initialized_ = false;
}

PhysicsCharacterItem::~PhysicsCharacterItem()
{
}

void PhysicsCharacterItem::setAzimuth( Float az )
{
    azimuth_ = az;
}

Float PhysicsCharacterItem::azimuth() const
{
    return azimuth_;
}

void PhysicsCharacterItem::enteredRefFrame( RefFrame * refFrame )
{
    PhysicsItem::enteredRefFrame( refFrame );
    geocentric_initialized_ = false;
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
    const Float az2 = azimuth_ * 0.5;
    const Float co2 = std::cos( az2 );
    const Float si2 = std::sin( az2 );
    Quaterniond azQ( co2, 0.0, si2, 0.0 );

    if ( geocentric_initialized_ )
        adjustGeocentric();
    else
        initGeocentric();

    azQ = surfQ_ * azQ;

    rb->SetRotationd( azQ );
    setQ( azQ );
}

void PhysicsCharacterItem::initGeocentric()
{
    RefFrame * rf = parent();
    if ( !rf )
        return;
    RefFrame * of = orbitingFrame( rf );
    if ( !of )
        return;
    State rs;
    of->relativeState( rf, rs );
    const Vector3d fromG = Vector3d( 0.0, 1.0, 0.0 );
    const Vector3d toG = -rs.r.Normalized();
    surfQ_.FromRotationTo( fromG, toG );
    geocentric_last_up_ = toG;
    geocentric_initialized_ = true;
}

void PhysicsCharacterItem::adjustGeocentric()
{
    RefFrame * rf = parent();
    if ( !rf )
        return;
    RefFrame * of = orbitingFrame( rf );
    if ( !of )
        return;
    State rs;
    of->relativeState( rf, rs );
    const Vector3d toG = -rs.r.Normalized();
    const Vector3d fromG = geocentric_last_up_;
    Quaterniond    adjQ;
    adjQ.FromRotationTo( fromG, toG );
    surfQ_ = adjQ * surfQ_;
    surfQ_.Normalize();
    geocentric_last_up_ = toG;
}

OrbitingFrame * PhysicsCharacterItem::orbitingFrame( RefFrame * rf )
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





