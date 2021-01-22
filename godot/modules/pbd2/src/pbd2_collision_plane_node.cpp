
#include "pbd2_collision_plane_node.h"

namespace Pbd
{

void PbdCollisionPlaneNode::_bind_methods()
{
    ClassDB::bind_method( D_METHOD( "set_size", "sz" ), &PbdCollisionPlaneNode::set_size );
    ClassDB::bind_method( D_METHOD( "get_size" ),      &PbdCollisionPlaneNode::get_size, Variant::REAL );

    ADD_PROPERTY( PropertyInfo( Variant::REAL, "size" ), "set_size", "get_size" );
}

PbdCollisionPlaneNode::PbdCollisionPlaneNode()
    : PbdCollisionObjectNode()
{
    collision_object = &collision_plane;
}

PbdCollisionPlaneNode::~PbdCollisionPlaneNode()
{
}


void PbdCollisionPlaneNode::set_size( real_t sz )
{
    collision_plane.set_size( sz );
}

real_t PbdCollisionPlaneNode::get_size() const
{
    return collision_plane.get_size();
}

}







