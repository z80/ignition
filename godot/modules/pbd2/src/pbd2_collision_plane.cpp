
#include "pbd2_collision_plane.h"
#include "pbd2_collision_sphere.h"
#include "pbd2_collisions.h"

#include "pbd2_broad_node.h"
#include "pbd2_rigid_body.h"
#include "pbd2_collision_utils.h"

namespace Pbd
{


CollisionPlane::CollisionPlane()
    : CollisionObject()
{
    obj_type = ObjectPlane;
    size_ = 100.0;
}

CollisionPlane::~CollisionPlane()
{
}

void CollisionPlane::set_size( Float sz )
{
    size_ = sz;
}

Float CollisionPlane::get_size() const
{
    return size_;
}
   
Float CollisionPlane::bounding_radius() const
{
    return size_;
}

bool CollisionPlane::inside( const BroadTreeNode * n, Float h ) const
{
    Pose pose_node;
    pose_node.r = n->center;
    const Pose pose_plane = pose_w();
    Pose pose_rel = pose_node / pose_plane;
    if ( rigid_body != nullptr )
    {
        const Vector3d vd = rigid_body->vel * k * h;
        const Float d = vd.Length();
        pose_rel.r.y_ -= d;
    }
    
    Box box;
    box.init( Vector3d( n->size2, n->size2, n->size2 ) );
    box.apply( pose_rel );
    for ( int i=0; i<8; i++ )
    {
        const BoxVertex & v = box.verts[i];
        if ( v.v.y_ < 0.0 )
            return true;
    }

    return false;
}

void CollisionPlane::intersect( CollisionObject * b, Vector<Vector3d> & ats, Vector<Vector3d> & depths )
{
    // Other objects collide with it.
    return;
}

}





