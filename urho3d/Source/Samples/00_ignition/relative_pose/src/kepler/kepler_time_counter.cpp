
#include "kepler_time_counter.h"
#include "kepler_mover.h"

namespace Ign
{

// Time in microseconds.
const Timestamp KeplerTimeCounter::ONE_SECOND  = 1000000;
const Float     KeplerTimeCounter::_ONE_SECOND = 1.0e-6;

KeplerTimeCounter::KeplerTimeCounter( Context * ctx )
    :LogicComponent( ctx )
{
    time_      = 0;
    dt_        = 0;
    xN_        = 1;
}

KeplerTimeCounter::~KeplerTimeCounter()
{

}

void KeplerTimeCounter::RegisterObject( Context * context )
{
    context->RegisterFactory<KeplerTimeCounter>();

    URHO3D_ATTRIBUTE_EX( "Global time",       Timestamp, time_, UpdateNodes, 0, AM_DEFAULT );
    URHO3D_ATTRIBUTE_EX( "Delta time",        Timestamp, dt_,   UpdateNodes, 0, AM_DEFAULT );
    URHO3D_ATTRIBUTE_EX( "Time acceleration", int, xN_,         UpdateNodes, 1, AM_DEFAULT );
}

void KeplerTimeCounter::Start()
{
    time_ = 0;
    dt_   = 0;
    xN_   = 1;
}

void KeplerTimeCounter::Update( float dtf )
{
    if ( xN_ == 0 )
        dt_ = 0;
    else
    {
        const Float accT = ( xN_ >= 0.0 ) ? static_cast<Float>(xN_) :
                                            (1.0/static_cast<Float>(-xN_));
        dt_ = static_cast<Timestamp>(
                 static_cast<Float>(dtf) * accT *
                 static_cast<Float>(ONE_SECOND)
             );
    }
    time_ += dt_;
}

void KeplerTimeCounter::UpdateNodes()
{
    Scene * s = GetScene();
    const Vector<SharedPtr<Component> > & comps = s->GetComponents();
    const unsigned qty = comps.Size();
    for ( unsigned i=0; i<qty; i++ )
    {
        Component * c = comps[i];
        KeplerMover * m = c->Cast<KeplerMover>();
        if ( !m )
            continue;
        // Advance node in time.
        m->IntegrateMotion( 0, dt_ );
    }

    // After all nodes are intergrated can compute relative
    // poses.
    // Compute position relative to physics world node.
    for ( unsigned i=0; i<qty; i++ )
    {
        Component * c = comps[i];
        KeplerMover * m = c->Cast<KeplerMover>();
        if ( !m )
            continue;
        // Advance node in time.
        m->ComputeRelativePose( 0 );
    }
}

void KeplerTimeCounter::SetTimeAcceleration( int xN )
{
    xN_ = xN;
}


}


