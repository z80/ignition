
#include "pbd2_collision_box_node.h"

namespace Pbd
{

void PbdCollisionBoxNode::_bind_methods()
{
    ClassDB::bind_method( D_METHOD( "set_size2", "sz2" ), &PbdCollisionBoxNode::set_size2 );
    ClassDB::bind_method( D_METHOD( "get_size2" ),        &PbdCollisionBoxNode::get_size2, Variant::VECTOR3 );

    ADD_PROPERTY( PropertyInfo( Variant::VECTOR3, "size2" ), "set_size2", "get_size2" );
}

PbdCollisionBoxNode::PbdCollisionBoxNode()
    : PbdCollisionObjectNode()
{
    collision_object = &collision_box;
}

PbdCollisionBoxNode::~PbdCollisionBoxNode()
{
}


void PbdCollisionBoxNode::set_size2( const Vector3 & sz2 )
{
    collision_box.set_size2( sz2 );
}

Vector3 PbdCollisionBoxNode::get_size2() const
{
    const Vector3d sz2 = collision_box.get_size2();
    return Vector3( sz2.x_, sz2.y_, sz2.z_ );
}

}







