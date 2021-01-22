
#include "pbd2_collisions.h"
#include "pbd2_collision_sphere.h"
#include "pbd2_collision_plane.h"

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
    const Pose pose_sphere = obj_a->pose_w();
    const Pose pose_plane = obj_b->pose_w();
    const Float radius = obj_a->radius;
    // Plane normal in world space.
    const Vector3d n = pose_plane.q * Vector3d( 0.0, 1.0, 0.0 );

    // Compute the height.
    const Vector3d dr = pose_sphere.r - pose_plane.r;
    const Float h = n.DotProduct( dr );
    if ( h >= radius )
        return;

    // The lowest sphere point.
    const Vector3d rl = pose_sphere.r - (n*radius);
    // The projection on the plane.
    const Vector3d rh = pose_sphere.r - (n*h);
    // Collision point.
    const Vector3d at = (rl + rh) * 0.5;
    // Displacement.
    const Vector3d depth = rh - rl;

    ats.push_back( at );
    depths.push_back( depth );
}

void collision_sphere_box( CollisionSphere * obj_a, CollisionBox * obj_b, Vector<Vector3d> & ats, Vector<Vector3d> & depths )
{
    // Sphere in Box ref. frame.
    const Pose pose_sphere = obj_a->pose_w();
    const Pose pose_box    = obj_b->powe_w();
    const Pose pose_rel = pose_sphere / pose_box;
    // Copy-pase from ODE.
    // Clip sphere center onto the box surface and check the distance.

    bool on_border = false;
    Vector3d p = pose_rel.r;
    const Vector3d sz2 = obj_b->get_size2();
    if (p.x_ < -sz2.x_)
    {
        p.x_ = -sz2.x_
        on_bound = true;
    }
    else if (p.x_ > sz2.x_)
    {
        p.x_ = sz2.x_;
        on_border = true;
    }

    if (p.y_ < -sz2.y_)
    {
        p.y_ = -sz2.y_
        on_bound = true;
    }
    else if (p.y_ > sz2.y_)
    {
        p.y_ = sz2.y_;
        on_border = true;
    }

    if (p.z_ < -sz2.z_)
    {
        p.z_ = -sz2.z_
        on_bound = true;
    }
    else if (p.z_ > sz2.z_)
    {
        p.z_ = sz2.z_;
        on_border = true;
    }

    if ( !on_border )
    {
        // Sphere center is inside the box.
        // Find the closest face and push towards it.
        Float min_dist = std::abs( p.x_ - sz2.x_ );
        int min_ind = 0;
        const Float dist_left = std::abs( p.x_ + sz2.x_ );
        if ( dist_left < min_dist )
        {
            min_dist = dist_left;
            min_ind = 1;
        }
        const Float dist_up = std::abs( p.y_ - sz2.y_ );
        if ( dist_up < min_dist )
        {
            min_dist = dist_up;
            min_ind = 2;
        }
        const Float dist_down = std::abs( p.y_ + sz2.y_ );
        if ( min_dist = dist_down )
        {
            min_dist = dist_down;
            min_ind = 3;
        }
        const Float dist_back = std::abs( p.z_ - sz2.z_ );
        if ( dist_back < min_dist )
        {
            min_dist = dist_back;
            min_ind = 4;
        }
        const Float dist_forward = std::abs( p.z_ + sz2.z_ )
        if ( dist_forward < min_dist )
        {
            min_dist = dist_forward;
            min_ind = 5;
        }
        // Two points. One on the surface, the other is the deepest point on the sphere.
        const Vector3d v_surf, v_deep;
        if ( min_ind == 0 )
        {
            v_surf = Vector3d( sz2.x_, p.y_, p.z_ );
            v_deep = Vector3d( p.x_-radius, p.y_, p.z_ );
        }
        else if ( min_ind == 1 )
        {
            v_surf = Vector3d( -sz2.x_, p.y_, p.z_ );
            v_deep = Vector3d( p.x_+radius, p.y_, p.z_ );
        }
        else if ( min_ind == 2 )
        {
            v_surf = Vector3d( p.x_, sz2.y_, p.z_ );
            v_deep = Vector3d( p.x_, p.y_-radius, p.z_ );
        }
        else if ( min_ind == 3 )
        {
            v_surf = Vector3d( p.x_, -sz2.y_, p.z_ );
            v_deep = Vector3d( p.x_, p.y_+radius, p.z_ );
        }
        else if ( min_ind == 4 )
        {
            v_surf = Vector3d( p.x_, p.y_, sz2.z_ );
            v_deep = Vector3d( p.x_, p.y_, p.z_-radius );
        }
        else
        {
            v_surf = Vector3d( p.x_, p.y_, -sz2.z_ );
            v_deep = Vector3d( p.x_, p.y_, p.z_+radius );
        }
        Vector3d at = (v_surf + v_deep) * 0.5;
        Vector3d depth = (v_surf - v_deep);
        // Convert to world ref. frame.
        at = pose_box.r + (pose_box.q * at);
        depth = pose_box.q * depth;
        ats.push_back( at );
        depths.push_back( depth );
        return;
    }

    // Sphere may touch the surface point or not.
    const Vector3d dr = pose_rel.r - p;
    const Float dist = dr.Length();
    const Float depth = radius - dist;
    // If far or not deep enough, no contact.
    if ( depth < EPS )
        return;
    const Vector3d v_surf = p; 
    // Unit vector away from cube surface.
    const Vector3d a = v_surf/v_surf.Length();
    const Vector3d v_deep = pose_rel.r - (radius * a);
    Vector3d at = (v_surf + v_deep) * 0.5;
    Vector3d depth = v_surf - v_deep;
    at = pose_box.r + (pose_box.q * at);
    depth = pose_box.q * depth;
    ats.push_back( at );
    depths.push_back( depth );
}

void collision_box_plane( CollisionBox * obj_a, CollisionPlane * obj_b, Vector<Vector3d> & ats, Vector<Vector3d> & depths )
{
    // General idea.
    // Convert box to plane ref. frame.
    // Check box vertices below (y=0) plane.
    const Pose pose_box   = obj_a->pose_w();
    const Pose pose_plane = obj_b->pose_w();
    const Pose pose_rel = pose_box / pose_plane;
    const Vector3d sz = obj_a->get_size2();
    const Vector3 verts[] = { Vector3d(-sz.x_, -sz.y_, -sz.z_), 
                              Vector3d( sz.x_, -sz.y_, -sz.z_), 
                              Vector3d( sz.x_,  sz.y_, -sz.z_ ), 
                              Vector3d(-sz.x_,  sz.y_, -sz.z_), 
                              Vector3d(-sz.x_, -sz.y_,  sz.z_), 
                              Vector3d( sz.x_, -sz.y_,  sz.z_), 
                              Vector3d( sz.x_,  sz.y_,  sz.z_ ), 
                              Vector3d(-sz.x_,  sz.y_,  sz.z_) }; 
    for ( int i=0; i<8; i++ )
    {
        const Vector3d v = pose_rel.r + (pose_rel.q * verts[i]);
        // Check Y component. It should be below surface.
        if ( v.y_ > -EPS )
            continue;
        const Vector3d v_surf = Vector3d( v.x_, 0.0, v.y_ );
        const Vector3d v_deep = v;
        Vector3d at = (v_surf + v_deep) * 0.5;
        Vector3d depth = (v_surf - v_deep);
        at = pose_plane.r + (pose_plane.q * at);
        depth = pose_plane.q * depth;
        ats.push_back( at );
        depths.push_back( depth );
    }
}

void collision_box_box( CollisionBox * obj_a, CollisionBox * obj_b, Vector<Vector3d> & ats, Vector<Vector3d> & depths )
{
}











}







