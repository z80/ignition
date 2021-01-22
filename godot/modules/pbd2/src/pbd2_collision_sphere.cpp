
#include "pbd2_collision_sphere.h"
#include "pbd2_collision_plane.h"
#include "pbd2_collisions.h"

namespace Pbd
{


CollisionSphere::CollisionSphere()
    : CollisionObject()
{
    radius = 1.0;
    obj_type = Sphere;
}

CollisionSphere::~CollisionSphere()
{
}
    
Float CollisionSphere::bounding_radius() const
{
    return radius;
}

void CollisionSphere::intersect( CollisionObject * b, Vector<Vector3d> & ats, Vector<Vector3d> & depths )
{
    CollisionObjectType tp = b->type();
    if ( tp == Sphere )
    {
        CollisionSphere * bs = dynamic_cast<CollisionSphere *>( b );
        if ( !bs )
            return;
        collision_sphere_sphere( this, bs, ats, depths );
    }
    else if ( tp == Plane )
    {
        CollisionPlane * bs = dynamic_cast<CollisionPlane *>( b );
        if ( !bs )
            return;
        collision_sphere_plane( this, bs, ats, depths );
    }
}

}





