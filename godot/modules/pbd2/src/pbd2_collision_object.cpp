
#include "pbd2_collision_object.h"
#include "pbd2_rigid_body.h"

namespace Pbd
{

CollisionObject::CollisionObject()
    : rigid_body( nullptr ), 
      k( 2.0 ), 
      obj_type( WrongType ) 
{
}

CollisionObject::~CollisionObject()
{
}

CollisionObject::CollisionObjectType CollisionObject::type() const
{
    return obj_type;
}

 
Vector3d CollisionObject::center() const
{
    Pose p = pose;
    if ( rigid_body != nullptr )
        p = rigid_body->pose * pose;

    return p.r;
}

Float CollisionObject::size2( Float h ) const
{
    Float sz = bounding_radius();
    if ( sz <= 0.0 )
        return sz;
    if ( rigid_body == nullptr )
        return sz;

    const Float v = rigid_body->vel.Length();
    sz += k*v*h;
    return sz;
}

Float CollisionObject::bounding_radius() const
{
    return -1.0;
}

void CollisionObject::intersect( CollisionObject * b, Vector<Vector3d> & ats, Vector<Vector3d> & depths )
{
    // By default do nothing of course.
}

Pose CollisionObject::pose_w() const
{
    Pose pw;
    if ( rigid_body != nullptr )
        pw = rigid_body->pose * pose;
    else
        pw = pose;
    return pw;
}



}










