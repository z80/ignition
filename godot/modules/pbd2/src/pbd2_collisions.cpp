
#include "pbd2_collisions.h"
#include "pbd2_collision_sphere.h"

namespace Pbd
{

static const Float EPS = 0.0001;

void collision_sphere_sphere( CollisionSphere * obj_a, CollisionSphere * obj_b, Vector<Vector3d> & ats, Vector<Vector3d> & depths )
{
    const Pose pose_a = obj_a->pose_w();
    const Pose pose_b = obj_b->pose_w();
    const Vector3d dr = pose_a.r - pose_b.r;
    const Float dist = dr.Length();
    const Float max_dist = obj_a->radius + obj_b->radius;
    if ( dist >= max_dist )
        return;

    if ( dist < EPS )
    {
        const Vector3d at = pose_a.r;
        const Vector3d depth = Vector3d::UP * max_dist;
        ats.push_back( at );
        depths.push_back( depth );
        return;
    }

    const Vector3d a = dr / dist;
    const Vector3d at = ( (pose_b.r + a*obj_b->radius) + (pose_a.r - a*obj_a->radius) ) * 0.5;
    const Vector3d depth = pose_b.r - pose_a.r + a*(obj_b->radius + obj_a->radius);
    ats.push_back( at );
    depths.push_back( depth );
}


void collision_sphere_plane( CollisionSphere * obj_a, CollisionPlane * obj_b, Vector<Vector3d> & ats, Vector<Vector3d> & depths )
{
    const Pose pose_a = obj_a->pose_w();
    const Pose pose_b = obj_b->pose_w();





}












}







