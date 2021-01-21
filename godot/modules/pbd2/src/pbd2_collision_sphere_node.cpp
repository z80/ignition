
#include "pbd2_collision_sphere_node.h"

namespace Pbd
{

void PbdCollisionSphereNode::_bind_methods()
{
    ClassDB::bind_method( D_METHOD( "set_radius", "r" ), &PbdCollisionSphereNode::set_radius );
    ClassDB::bind_method( D_METHOD( "get_radius" ),      &PbdCollisionSphereNode::get_radius, Variant::REAL );

    ADD_PROPERTY( PropertyInfo( Variant::REAL, "radius" ), "set_radius", "get_radius" );
}

PbdCollisionSphereNode::PbdCollisionSphereNode()
    : PbdCollisionObjectNode()
{
    collision_object = &collision_sphere;
}

PbdCollisionSphereNode::~PbdCollisionSphereNode()
{
}


void PbdCollisionSphereNode::set_radius( real_t r )
{
    collision_sphere.radius = r;
}

real_t PbdCollisionSphereNode::get_radius() const
{
    return collision_sphere.radius;
}

}







