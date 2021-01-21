
#ifndef __PBD2_COLLISION_OBJECT_NODE_H_
#define __PBD2_COLLISION_OBJECT_NODE_H_

#include "scene/3d/spatial.h"
#include "pbd2_collision_object.h"


namespace Pbd
{

class PbdCollisionObjectNode: public Spatial
{
    GDCLASS( PbdCollisionObjectNode, Spatial );
protected:
    static void _bind_methods();
    void _notifications( int p_what );
public:
    PbdCollisionObjectNode();
    ~PbdCollisionObjectNode();


    CollisionObject * collision_object;
};


}


#endif











