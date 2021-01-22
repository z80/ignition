
#include "pbd2_collision_plane.h"
#include "pbd2_collision_sphere.h"
#include "pbd2_collisions.h"

namespace Pbd
{


CollisionPlane::CollisionPlane()
    : CollisionObject()
{
    obj_type = Plane;
    size_ = 100.0;
}

CollisionPlane::~CollisionPlane()
{
}
    
Float CollisionPlane::bounding_radius() const
{
    return size_;
}

void CollisionPlane::set_size( Float sz )
{
    size_ = sz;
}

Float CollisionPlane::get_size() const
{
    return size_;
}

void CollisionPlane::intersect( CollisionObject * b, Vector<Vector3d> & ats, Vector<Vector3d> & depths )
{
    // Other objects collide with it.
    return;
}

}





