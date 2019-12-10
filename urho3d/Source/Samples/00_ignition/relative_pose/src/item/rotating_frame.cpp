
#include "rotating_frame.h"

namespace Ign
{

void RotatingFrame::RegisterComponent( Context * context )
{
    context->RegisterFactory<RotatingFrame>();
    URHO3D_COPY_BASE_ATTRIBUTES( EvolvingFrame );

    URHO3D_ATTRIBUTE( "Ow", double, orientation_.w_, poseChanged, 0.0, AM_DEFAULT );
    URHO3D_ATTRIBUTE( "Ox", double, orientation_.x_, poseChanged, 0.0, AM_DEFAULT );
    URHO3D_ATTRIBUTE( "Oy", double, orientation_.y_, poseChanged, 0.0, AM_DEFAULT );
    URHO3D_ATTRIBUTE( "Oz", double, orientation_.z_, poseChanged, 0.0, AM_DEFAULT );

    URHO3D_ATTRIBUTE( "Period", Timestamp, period_, 0, AM_DEFAULT );
    URHO3D_ATTRIBUTE( "Phase",  Timestamp, phase_,  0, AM_DEFAULT );
 
}

RotatingFrame::RotatingFrame( Context * context )
{
}

RotatingFrame::~RotatingFrame()
{
}

void RotatingFrame::evolveStep( Timestamp ticks_dt )
{
    if ( period_ == 0 )
    {
        st.q_ = Quaterniond();
    }
    else
    {
        const Float angle2 = 0.5 * PI2 * static_cast<Float>( phase_ ) / static_cast<Float>( period_ );
        const Quaterniond q( std::cos( angle2 ), 0.0, std::sin( angle2 ), 0.0 );
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


}




