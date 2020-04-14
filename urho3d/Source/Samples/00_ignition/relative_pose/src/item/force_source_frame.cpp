
#include "force_source_frame.h"
#include "physics_item.h"

namespace Ign
{

void ForceSourceFrame::RegisterComponent( Context * context )
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

void ForceSourceFrame::ApplyForces( PhysicsItem * receiver )
{
    const bool producesForces = ProducesForces();
    if ( producesForces )
    {
        State st;
        receiver->relativeState( this, st );
        Vector3d F, P;
        ComputeForces( receiver, st, F, P );
        // Forces are in local ref. frame.
        // But are needed in receiver's parent ref. frame.
        // 1) Convert to receiver's ref. frame.
        const Quaterniond toReceiverQ = st.q.Inverse();
        F = toReceiverQ * F;
        P = toReceiverQ * P;
        const Vector3d debugDrawF = F;
        // 2) Convert to parent ref. frame.
        const Quaterniond & q = receiver->relQ();
        F = q * F;
        P = q * P;
        // Apply forces to the receiver.
        // .........
        RigidBody2 * rb = receiver->rigidBody();
        rb->ApplyForce( Vector3( F.x_, F.y_, F.z_ ) );
        rb->ApplyTorque( Vector3( P.x_, P.y_, P.z_ ) );

        AirMesh & am = receiver->airMesh();
        ForceApplied fa;
        fa.at = Vector3d( 2.0, 0.0, 0.0 );
        fa.Fn = debugDrawF;
        am.forces_.Push( fa );
    }

    const bool recursive = Recursive();
    if ( !recursive )
        return;

    RefFrame * p = parent();
    if ( !p )
        return;

    ForceSourceFrame * fs = p->Cast<ForceSourceFrame>();
    if ( fs )
        fs->ApplyForces( receiver );
}

void ForceSourceFrame::ComputeForces( PhysicsItem * receiver, const State & st, Vector3d & F, Vector3d & P ) const
{
}


}







