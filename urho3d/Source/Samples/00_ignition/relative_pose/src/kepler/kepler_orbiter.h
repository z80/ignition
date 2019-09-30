
#ifndef __KEPLER_ORBITER_H_
#define __KEPLER_ORBITER_H_

#include <Urho3D/Urho3DAll.h>
#include "kepler_mover.h"

using namespace Urho3D;

namespace Ign
{

class KeplerOrbiter: public KeplerMover
{
    URHO3D_OBJECT( KeplerOrbiter, KeplerMover )
public:
    KeplerOrbiter( Context * ctx );
    virtual ~KeplerOrbiter();

    /// Register object factory.
    static void RegisterObject( Context * context );

    void IntegrateMotion( ItemNode * world, Timestamp dt ) override;
    void ComputeRelativePose( ItemNode * world ) override;

    void launch( Float GM, Float a, Float e, Float Omega=0.0, Float I=0.0, Float omega=0.0, Float E=0.0 );
    // Computes orbit elements based on position and velocity.
    bool launch( const Vector3d & v );
    bool launch( const Vector3d & v, Float GM );
    // Stop using Kepler orbit.
    void stop();
    // Obtain current position and velocity.
    virtual Vector3d relR() const;
    virtual Vector3d relV() const;

public:
    static const Float TIME_T;
    // Small number for solving.
    static const Float eps;
    static const int   iters;
    static const Float minAngularMomentum;
public:
    // Counting time.
    Float timeLow;
    Float timeHigh;

    bool active;
    Float GM;
    // Eccentricity
    Float e;
    // Semimajor axis
    Float a;
    // Semi-latus rectum.
    Float l;
    // Eccentric anomaly
    Float E;
    // Current anomaly
    Float f;
    // Period
    Float P;
    // Current time.
    Float tau;

    // Orbit unit vectors.
    Vector3d ex, ey;
};

}

#endif



