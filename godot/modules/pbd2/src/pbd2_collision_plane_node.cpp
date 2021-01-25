
#include "pbd2_collision_plane_node.h"

namespace Pbd
{

void PbdCollisionPlaneNode::_bind_methods()
{
}

PbdCollisionPlaneNode::PbdCollisionPlaneNode()
    : PbdCollisionObjectNode()
{
    collision_object = &collision_plane;
}

PbdCollisionPlaneNode::~PbdCollisionPlaneNode()
{
}



}







