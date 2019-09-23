
#ifndef __KEPLER_MOVER_H_
#define __KEPLER_MOVER_H_

#include <Urho3D/Urho3DAll.h>
#include "item_node.h"

using namespace Urho3D;

namespace Ign
{

class KeplerMover: public ItemNode
{
    URHO3D_OBJECT( KeplerMover, ItemNode )
public:
    KeplerMover( Context * ctx );
    virtual ~KeplerMover();

    virtual void Start() override;
    virtual void Update( float dt );

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
    // Small number for
    static const Float eps;
    static const int   iters;

    SharedPtr<GameData> gameData;
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



