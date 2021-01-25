
#ifndef __PBD2_COLLISION_PLANE_NODE_H_
#define __PBD2_COLLISION_PLANE_NODE_H_

#include "pbd2_collision_object_node.h"
#include "pbd2_collision_plane.h"

namespace Pbd
{

class PbdCollisionPlaneNode: public PbdCollisionObjectNode
{
    GDCLASS( PbdCollisionPlaneNode, PbdCollisionObjectNode );

protected:
    static void _bind_methods();

public:
    PbdCollisionPlaneNode();
    ~PbdCollisionPlaneNode();

public:
    CollisionPlane collision_plane;
};



}


#endif





