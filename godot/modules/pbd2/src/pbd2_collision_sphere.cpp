
#include "pbd2_collision_sphere.h"
#include "pbd2_collision_box.h"
#include "pbd2_collision_plane.h"
#include "pbd2_collisions.h"

#include "pbd2_broad_node.h"
#include "pbd2_rigid_body.h"

namespace Pbd
{


CollisionSphere::CollisionSphere()
    : CollisionObject()
{
    radius = 1.0;
    obj_type = ObjectSphere;
}

CollisionSphere::~CollisionSphere()
{
}
    
Float CollisionSphere::bounding_radius() const
{
    return radius;
}


bool CollisionSphere::inside( BroadTreeNode * n, Float h ) const
{
    const Pose p = pose_w();
    Float r = radius;
    if ( rigid_body != nullptr )
    {
        const Vector3d v = rigid_body->vel;
        const Float d = v.Length() * k * h;
        r += d;
    }
    const Vector3d r_rel = n->center - p.r;
    if ( std::abs(r_rel.x_) > r )
        return false;
    if ( std::abs(r_rel.y_) > r )
        return false;
    if ( std::abs(r_rel.z_) > r )
        return false;
    return true;
}

void CollisionSphere::intersect( CollisionObject * b, Vector<Vector3d> & ats, Vector<Vector3d> & depths )
{
    CollisionObjectType tp = b->type();
    if ( tp == ObjectSphere )
    {
        CollisionSphere * bs = dynamic_cast<CollisionSphere *>( b );
        if ( !bs )
            return;
        collision_sphere_sphere( this, bs, ats, depths );
    }
    if ( tp == ObjectBox )
    {
        CollisionBox * bs = dynamic_cast<CollisionBox *>( b );
        if ( !bs )
            return;
        collision_sphere_box( this, bs, ats, depths );
    }
    else if ( tp == ObjectPlane )
    {
        CollisionPlane * bs = dynamic_cast<CollisionPlane *>( b );
        if ( !bs )
            return;
        collision_sphere_plane( this, bs, ats, depths );
    }
}

}





