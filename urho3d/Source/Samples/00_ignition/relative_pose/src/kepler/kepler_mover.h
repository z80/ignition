
#ifndef __KEPLER_MOVER_H_
#define __KEPLER_MOVER_H_

#include "Urho3D/Urho3DAll.h"
#include "item_node.h"

using namespace Urho3D;

namespace Ign
{

class KeplerMover
{
    URHO3D_OBJECT( KeplerMover, ItemNode )
public:
    KeplerMover( Context * ctx );
    virtual ~KeplerMover();

    /// Register object factory.
    static void RegisterObject( Context * context);


    /// Integrate motion if not direct child of
    /// node provided. Which is physics world's node.
    virtual void IntegrateMotion( ItemNode * world, Timestamp dt );

    /// Compute Relative pose
    virtual void ComputeRelativePose( ItemNode * world );

public:
    /// Dynamic parameters relative to physics world node.
    Vector3d worldR_, worldV_, worldW_;
    Quaterniond worldQ_;

    /// Right reference frame unit vectors.
    /// These are for orbits and so on.
    static const Matrix3d TO_SPACE,
                          RO_GAME;
};

}


#endif




