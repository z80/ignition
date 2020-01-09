
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

}

void PhysicsCharacterItem::orientRigidBody( RigidBody2 * rb )
{
    RefFrame * rf = parent();
    OrbitingFrame * of = orbitingFrame( rf );
    if ( !of )
        return;
    State rs;
    of->relativeState( this, rs );
    const Vector3d wantedG( 0.0, -1.0, 0.0 );
    const Vector3d actualG = rs.r.Normalized();
    const Vector3d cross = wantedG.CrossProduct( actualG );
    const Float si = cross.Length();
    static const Float EPS = 0.0001;
    Quaterniond baseQ;
    if ( si < EPS )
    {
        baseQ = Quaterniond::IDENTITY;
    }
    else
    {
        const Float co = wantedG.DotProduct( actualG );
        const Float angle = std::atan2( si, co );
        const Float angle_2 = angle * 0.5;
        const Float co2 = std::cos( angle_2 );
        const Float si2 = std::sin( angle_2 );
        const Vector3d e = cross / si;
        baseQ = Quaterniond( co2, si2*e.x_, si2*e.y_, si2*e.z_ );
    }
    const Float az_2 = azimuth_ * 0.5;
    const Float azSi2 = std::sin( az_2 );
    const Float azCo2 = std::cos( az_2 );
    const Quaterniond azQ( azCo2, 0.0, azSi2, 0.0 );
    const Quaterniond q = baseQ * azQ;

    rb->SetRotationd( q );
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





