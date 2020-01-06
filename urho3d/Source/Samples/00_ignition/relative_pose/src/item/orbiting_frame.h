
#ifndef __ORBITING_FRAME_H_
#define __ORBITING_FRAME_H_

#include "evolving_frame.h"

using namespace Urho3D;

namespace Ign
{

struct OrbitDesc
{
    Float    gm;
    Float    eccentricity;
    Float    semimajorAxis;
    Vector3d ex, 
             ey;

    // For hyperbolic and elliptic.
    Float     eccentricAnomaly;
    // For parabolic.
    Float     semiLatusRectum;
    // These two are for information only.
    Timestamp periapsisTime;
    Timestamp orbitalPeriod;
};

class OrbitingFrame: public EvolvingFrame
{
    URHO3D_OBJECT( OrbitingFrame, EvolvingFrame )
public:
    static void RegisterObject( Context * context );

    OrbitingFrame( Context * context );
    virtual ~OrbitingFrame();

    void evolveStep( Timestamp ticks_dt ) override;

    // Implementation of ForceSourceFrame to 
    // produce Coriolis and Centrifugal forces.
    bool Recursive() const override;
    bool ProducesForces() const override;
    void ComputeForces( PhysicsItem * receiver, const State & st, Vector3d & F, Vector3d & P ) const override;

    void SetGM( Float GM );
    Float GM() const;

    /// Body radius.
    void SetR( Float R );
    Float R() const;

    bool Launch( const Vector3d & r, const Vector3d & v );

public:
    Float GM_;
    Float R_;
    OrbitDesc orbitDesc;
    bool active_;
};




}





#endif



