
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

void PhysicsCharacterItem::refStateChanged()
{
    PhysicsItem::refStateChanged();

    consistencyCheck();

    // Here it is for debugging...
    // Here check for proper parent after merge.
    {
        Environment * e = Environment::environment( context_ );
        if ( !e )
            return;
        const ClientDesc & cd = e->clientDesc();
        //if ( cd.id_ < 1 )
        //    return;

        RefFrame * p = parent();
        if ( !p )
            URHO3D_LOGINFO( String( "No parent" ) );
        else
        {
            static int counter = 0;
            static bool en = true;
            if ( counter >= 201 )
            {
                counter -= 201;
                const Vector3d at = relR();
                String stri = String( "name: " ) + name_ + 
                    String(", parent name: ") + p->name() + 
                    String(", r: (") + String(at.x_) + 
                    String(", ") + String(at.y_) + 
                    String(", ") + String(at.z_) + String(")");
                const bool node_enabled = visual_node_->IsEnabled();
                stri += String( ", node_enabled: " ) + String( node_enabled ? "True" : "False" );
                Component * c = visual_node_->GetComponent<StaticModel>();
                if ( c )
                {
                    const bool comp_enabled = c->IsEnabled();
                    stri += String( ", comp_enabled: " ) + String( node_enabled ? "True" : "False" );
                }
                Node * np = visual_node_->GetParent();
                stri += String( ", parentNode: " ) + String( (long long)np ); 
                URHO3D_LOGINFO( stri );
                if ( name_ == "CharacterCube object #1" )
                {
                    visual_node_->SetEnabled( en );
                    if ( c )
                        c->SetEnabled( en );
                    en = !en;
                }
            }
            counter += 1;
        }
    }
}

void PhysicsCharacterItem::enteredRefFrame( unsigned refFrameId )
{
    PhysicsItem::enteredRefFrame( refFrameId );
    geocentric_initialized_ = false;
}

bool PhysicsCharacterItem::consistencyCheck()
{
    const bool baseOk = PhysicsItem::consistencyCheck();
    return baseOk;
}

void PhysicsCharacterItem::setupPhysicsContent( RigidBody2 * rb, CollisionShape2 * cs )
{
    consistencyCheck();

    PhysicsItem::setupPhysicsContent( rb, cs );

    consistencyCheck();

    // All rotational degrees of freedom are disabled.
    rb->SetAngularFactor( Vector3::ZERO );

    consistencyCheck();
}

void PhysicsCharacterItem::physicsUpdate( RigidBody2 * rb )
{
    orientRigidBody( rb );
}

void PhysicsCharacterItem::orientRigidBody( RigidBody2 * rb )
{
    consistencyCheck();

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
    rb->SetAngularVelocity( Vector3::ZERO );

    consistencyCheck();
}

void PhysicsCharacterItem::initGeocentric()
{
    RefFrame * prt = parent();
    if ( !prt )
        return;
    RefFrame * of = orbitingFrame( prt );
    if ( !of )
        return;
    State rs;
    of->relativeState( parent_id_, rs );
    const Vector3d fromG = Vector3d( 0.0, 1.0, 0.0 );
    const Vector3d toG = -rs.r.Normalized();
    surfQ_.FromRotationTo( fromG, toG );
    geocentric_last_up_ = toG;
    geocentric_initialized_ = true;
}

void PhysicsCharacterItem::adjustGeocentric()
{
    RefFrame * prt = parent();
    if ( !prt )
        return;
    RefFrame * of = orbitingFrame( prt );
    if ( !of )
        return;
    State rs;
    of->relativeState( parent_id_, rs );
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





