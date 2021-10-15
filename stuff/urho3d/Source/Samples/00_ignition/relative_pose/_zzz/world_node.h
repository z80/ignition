
#ifndef __WORLD_NODE_H_
#define __WORLD_NODE_H_

#include "item_node.h"
#include "data_types.h"

namespace Ign
{

class KeplerTimeCounter;

/// TODO: Later will need to inherit from KeplerOrbiter
/// to be able to manipulate objects at orbit.
/// This thing decides when to teleport.
/// When to include objects into world or take them out.
class WorldNode: public ItemNode
{
    URHO3D_OBJECT( WorldNode, ItemNode )
public:
    WorldNode( Context * ctx );
    ~WorldNode();

    static void RegisterObject( Context * context );

    /// Check if needs to adjust origin to currently tracked object.
    void CheckTeleport();
    /// Should be called by KeplerTimeCounter.
    void IncludeRemoveObjects();

public:
    Float distInclude_,
          distRemove_;
};


}




#endif



