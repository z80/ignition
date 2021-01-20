
#include "pbd2_collision_sphere.h"

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

    }
}

}





