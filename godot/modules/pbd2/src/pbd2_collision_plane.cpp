
#include "pbd2_collision_plane.h"
#include "pbd2_collision_sphere.h"
#include "pbd2_collisions.h"

namespace Pbd
{


CollisionPlane::CollisionPlane()
    : CollisionObject()
{
    radius = 1.0;
    obj_type = Sphere;
}

CollisionPlane::~CollisionPlane()
{
}
    
Float CollisionPlane::bounding_radius() const
{
    return radius;
}

void CollisionPlane::intersect( CollisionObject * b, Vector<Vector3d> & ats, Vector<Vector3d> & depths )
{
    CollisionObjectType tp = b->type();
    if ( tp == Sphere )
    {
        CollisionSphere * bs = dynamic_cast<CollisionSphere *>( b );
        if ( !bs )
            return;
        collision_plane_sphere( this, bs, ats, depths );
    }
}

}





