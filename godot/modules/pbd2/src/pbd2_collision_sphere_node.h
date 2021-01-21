
#ifndef __PBD2_COLLISION_SPHERE_NODE_H_
#define __PBD2_COLLISION_SPHERE_NODE_H_

#include "pbd2_collision_object_node.h"
#include "pbd2_collision_sphere.h"

namespace Pbd
{

class PbdCollisionSphereNode: public PbdCollisionObjectNode
{
    GDCLASS( PbdCollisionSphereNode, PbdCollisionObjectNode );

protected:
    static void _bind_methods();

public:
    PbdCollisionSphereNode();
    ~PbdCollisionSphereNode();

    void set_radius( real_t r );
    real_t get_radius() const;


public:
    CollisionSphere collision_sphere;
};



}


#endif





