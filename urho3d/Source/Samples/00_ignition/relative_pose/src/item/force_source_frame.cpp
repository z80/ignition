
#include "force_source_frame.h"
#include "physics_item.h"

namespace Ign
{

void ForceSourceFrame::RegisterObject( Context * context )
{
    context->RegisterFactory<ForceSourceFrame>();
    URHO3D_COPY_BASE_ATTRIBUTES( RefFrame );
}

ForceSourceFrame::ForceSourceFrame( Context * context )
    : RefFrame( context )
{
}

ForceSourceFrame::~ForceSourceFrame()
{
}

bool ForceSourceFrame::Recursive() const
{
    return true;
}

bool ForceSourceFrame::ProducesForces() const
{
    return false;
}

void ForceSourceFrame::ApplyForces( PhysicsItem * receiver ) const
{
    const bool producesForces = ProducesForces();
    if ( producesForces )
    {
        if ( !receiver )
            return;
        RigidBody2 * rb = receiver->rigidBody();
        if ( !rb )
            return;

        State st;
        receiver->relativeState( this, st );
        Vector3d F, P;
        ComputeForces( receiver, st, F, P );
        // Forces are in local ref. frame.
        // But are needed in receiver's parent ref. frame.
        // 1) Convert to receiver's ref. frame.
        F = st.q * F;
        P = st.q * P;
        // 2) Convert to parent ref. frame.
        const Quaterniond & q = receiver->relQ();
        F = q * F;
        P = q * P;
        // Apply forces to the receiver.
        // .........
        rb->ApplyForce( Vector3( F.x_, F.y_, F.z_ ) );
        rb->ApplyTorque( Vector3( P.x_, P.y_, P.z_ ) );
    }

    const bool recursive = Recursive();
    if ( !recursive )
        return;

    RefFrame * p = parent();
    if ( !p )
        return;

    ForceSourceFrame * fs = p->Cast<ForceSourceFrame>();
    if ( fs )
        ApplyForces( receiver );
}

void ForceSourceFrame::ComputeForces( PhysicsItem * receiver, const State & st, Vector3d & F, Vector3d & P ) const
{
}


}







