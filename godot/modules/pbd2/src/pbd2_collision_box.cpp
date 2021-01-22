
#include "pbd2_collision_box.h"
#include "pbd2_collision_plane.h"
#include "pbd2_collisions.h"

namespace Pbd
{


CollisionBox::CollisionBox()
    : CollisionObject()
{
    size2 = Vector3d( 1.0, 1.0, 1.0 );
    obj_type = Box;
}

CollisionBox::~CollisionBox()
{
}
    
Float CollisionBox::bounding_radius() const
{
    return size2.Length() * 0.5;
}

void CollisionBox::intersect( CollisionObject * b, Vector<Vector3d> & ats, Vector<Vector3d> & depths )
{
    CollisionObjectType tp = b->type();
    if ( tp == Box )
    {
        CollisionBox * bs = dynamic_cast<CollisionBox *>( b );
        if ( !bs )
            return;
        collision_box_box( this, bs, ats, depths );
    }
    if ( tp == Plane )
    {
        CollisionPlane * bs = dynamic_cast<CollisionPlane *>( b );
        if ( !bs )
            return;
        collision_box_plane( this, bs, ats, depths );
    }
}

void CollisionBox::set_size2( const Vector3d sz )
{
    size2 = sz;
}

Vector3d CollisionBox::get_size2() const
{
    return size2;
}


}





