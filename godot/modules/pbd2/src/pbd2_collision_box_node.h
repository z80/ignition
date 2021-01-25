
#ifndef __PBD2_COLLISION_BOX_NODE_H_
#define __PBD2_COLLISION_BOX_NODE_H_

#include "pbd2_collision_object_node.h"
#include "pbd2_collision_box.h"

namespace Pbd
{

class PbdCollisionBoxNode: public PbdCollisionObjectNode
{
    GDCLASS( PbdCollisionBoxNode, PbdCollisionObjectNode );

protected:
    static void _bind_methods();

public:
    PbdCollisionBoxNode();
    ~PbdCollisionBoxNode();

    void set_size( const Vector3 & sz );
    Vector3 get_size() const;


public:
    CollisionBox collision_box;
};



}


#endif





