
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

    /// Integrate motion if not direct child of
    /// node provided. Which is physics world's node.
    virtual void IntegrateMotion( ItemNode * world, Timestamp dt );

    /// Compute Relative pose
    virtual void ComputeRelativePose( ItemNode * world );
};

}


#endif




