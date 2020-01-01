
#ifndef __ROTATING_FRAME_H_
#define __ROTATING_FRAME_H_

#include "Urho3D/Urho3DAll.h"
#include "evolving_frame.h"

namespace Ign
{

class RotatingFrame: public EvolvingFrame
{
    URHO3D_OBJECT( RotatingFrame, EvolvingFrame )
public:
    static void RegisterComponent( Context * context );

    RotatingFrame( Context * context );
    virtual ~RotatingFrame();

    void evolveStep( Timestamp ticks_dt );

    void SetPeriod( Timestamp period );
    Timestamp GetPeriod() const;

    void SetPhase( Timestamp phase );
    Timestamp GetPhase() const;

    // Implementation of ForceSourceFrame to 
    // produce Coriolis and Centrifugal forces.
    bool Recursive() const override;
    bool ProducesForces() const override;
    void ComputeForces( PhysicsItem * receiver, const State & st, Vector3d & F, Vector3d & P ) const override;



public:
    Timestamp   period_;
    Timestamp   phase_;
    Quaterniond orientation_;
};

}


#endif



