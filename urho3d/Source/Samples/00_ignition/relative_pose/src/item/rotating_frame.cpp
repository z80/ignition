
#include "rotating_frame.h"
#include "physics_item.h"
#include "settings.h"

namespace Ign
{

void RotatingFrame::RegisterComponent( Context * context )
{
    context->RegisterFactory<RotatingFrame>();
    URHO3D_COPY_BASE_ATTRIBUTES( EvolvingFrame );

    URHO3D_ATTRIBUTE( "Ow", double, orientation_.w_, 0.0, AM_DEFAULT );
    URHO3D_ATTRIBUTE( "Ox", double, orientation_.x_, 0.0, AM_DEFAULT );
    URHO3D_ATTRIBUTE( "Oy", double, orientation_.y_, 0.0, AM_DEFAULT );
    URHO3D_ATTRIBUTE( "Oz", double, orientation_.z_, 0.0, AM_DEFAULT );

    URHO3D_ATTRIBUTE( "Period", Timestamp, period_, 0, AM_DEFAULT );
    URHO3D_ATTRIBUTE( "Phase",  Timestamp, phase_,  0, AM_DEFAULT );
}

RotatingFrame::RotatingFrame( Context * context )
    : EvolvingFrame( context )
{
	phase_ = 0;
	period_ = Settings::ticks(60.0);
	orientation_ = Quaterniond::IDENTITY;
    setName( "RotatingFrame" );
}

RotatingFrame::~RotatingFrame()
{
}

void RotatingFrame::evolveStep( Timestamp ticks_dt )
{
    if ( period_ == 0 )
    {
        phase_ = 0;
        st_.q  = Quaterniond();
    }
    else
    {
        phase_ += ticks_dt;
        if ( phase_ >= period_ )
            phase_ -= period_;
        const Float angle2 = 0.5 * PI2 * static_cast<Float>( phase_ ) / static_cast<Float>( period_ );
        const Quaterniond q( std::cos( angle2 ), 0.0, -std::sin( angle2 ), 0.0 );
        st_.q = orientation_ * q;
    }

    MarkNetworkUpdate();
}

void RotatingFrame::SetPeriod( Timestamp period )
{
    period_ = period;

    MarkNetworkUpdate();
}

Timestamp RotatingFrame::GetPeriod() const
{
    return period_;
}

void RotatingFrame::SetPhase( Timestamp phase )
{
    phase_ = phase;

    MarkNetworkUpdate();
}

Timestamp RotatingFrame::GetPhase() const
{
    return phase_;
}

bool RotatingFrame::Recursive() const
{
    return true;
}

bool RotatingFrame::ProducesForces() const
{
    return true;
}

void RotatingFrame::ComputeForces( PhysicsItem * receiver, const State & st, Vector3d & F, Vector3d & P ) const
{
    RigidBody2 * rb = receiver->rigidBody();
    Float m = rb->GetMass();
    // Angular velocity.
    const Float periodSecs = Settings::secs( period_ );
    const Vector3d W( 0.0, PI2/periodSecs, 0.0 );
    // Coriolis forse is "-2.0*m*(W x V)".
    const Vector3d F_coriolis = -2.0*m*( W.CrossProduct( st.v ) );
    // Centrifugal force is "-m*W x (W x R)"
    const Vector3d F_centrifugal = -m*( W.CrossProduct( W.CrossProduct( st.r ) ) );
    F = F_coriolis + F_centrifugal;
}








}




