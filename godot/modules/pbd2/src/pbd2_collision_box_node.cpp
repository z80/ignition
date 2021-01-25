
#include "pbd2_collision_box_node.h"

namespace Pbd
{

void PbdCollisionBoxNode::_bind_methods()
{
    ClassDB::bind_method( D_METHOD( "set_size", "sz" ), &PbdCollisionBoxNode::set_size );
    ClassDB::bind_method( D_METHOD( "get_size" ),        &PbdCollisionBoxNode::get_size, Variant::VECTOR3 );

    ADD_PROPERTY( PropertyInfo( Variant::VECTOR3, "size" ), "set_size", "get_size" );
}

PbdCollisionBoxNode::PbdCollisionBoxNode()
    : PbdCollisionObjectNode()
{
    collision_object = &collision_box;
}

PbdCollisionBoxNode::~PbdCollisionBoxNode()
{
}


void PbdCollisionBoxNode::set_size( const Vector3 & sz )
{
    collision_box.set_size( sz );
}

Vector3 PbdCollisionBoxNode::get_size() const
{
    const Vector3d sz = collision_box.get_size();
    return Vector3( sz.x_, sz.y_, sz.z_ );
}

}







