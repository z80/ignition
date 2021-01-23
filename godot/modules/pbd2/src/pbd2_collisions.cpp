
#include "pbd2_collisions.h"
#include "pbd2_collision_sphere.h"
#include "pbd2_collision_box.h"
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
    const Pose pose_box    = obj_b->pose_w();
    const Pose pose_rel = pose_sphere / pose_box;
    const Float radius = obj_a->radius;
    // Copy-pase from ODE.
    // Clip sphere center onto the box surface and check the distance.

    bool on_border = false;
    Vector3d p = pose_rel.r;
    const Vector3d sz2 = obj_b->get_size2();
    if (p.x_ < -sz2.x_)
    {
        p.x_ = -sz2.x_;
        on_border = true;
    }
    else if (p.x_ > sz2.x_)
    {
        p.x_ = sz2.x_;
        on_border = true;
    }

    if (p.y_ < -sz2.y_)
    {
        p.y_ = -sz2.y_;
        on_border = true;
    }
    else if (p.y_ > sz2.y_)
    {
        p.y_ = sz2.y_;
        on_border = true;
    }

    if (p.z_ < -sz2.z_)
    {
        p.z_ = -sz2.z_;
        on_border = true;
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
        const Float dist_forward = std::abs( p.z_ + sz2.z_ );
        if ( dist_forward < min_dist )
        {
            min_dist = dist_forward;
            min_ind = 5;
        }
        // Two points. One on the surface, the other is the deepest point on the sphere.
        Vector3d v_surf, v_deep;
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
    Vector3d v_at = (v_surf + v_deep) * 0.5;
    Vector3d v_depth = v_surf - v_deep;
    v_at = pose_box.r + (pose_box.q * v_at);
    v_depth = pose_box.q * v_depth;
    ats.push_back( v_at );
    depths.push_back( v_depth );
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
    const Vector3d verts[] = { Vector3d(-sz.x_, -sz.y_, -sz.z_), 
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


enum FaceId { BACK, RIGHT, FORWARD, LEFT, DOWN, UP };
struct BoxVertex
{
    Vector3d v;
    bool inside;
    FaceId face_id;
    Float  depth;
};
struct BoxFace
{
    FaceId id;
    // Outwards normal.
    Vector3d n;
    // Face center.
    Vector3d center;
    // Two ortogonal vectors along face dimensions.
    Vector3d x, y;
    // Appropriate half sizes.
    Float sz_x, sz_y;
    bool intersects( const CubeVertex & a, const CubeVertex & b, Vector3d & at, Vector3d & depth ) const
    {
        const Vector3d b_a = b-a;
        const Float den = n.DotProduct( b_a );
        if ( std::abs(den) < EPS )
            return false;
        const Float num = n.DotProduct(center - a);
        const Float t = num / den;
        if ( (t < 0.0) || (t > 1.0) )
            return false;
        const Vector3d v_at = a + (b_a*t);
        const Vector3d d_at = v_at - center;
        const Float vx = x.DotProduct( d_at );
        if ( sz_x - std::abs(vx) < EPS )
            return false;
        const Float vy = y.DotProduct( d_at );
        if ( sz_y - std::abs(vy) < EPS )
            return false;
        // Determine the smallest distance to the edge.
        Float min_d = std::abs( -sz_x - vx );
        depth = -x;
        const Float dx_p = std::abs( sz_x - vx );
        if ( min_d > dx_p )
        {
            min_d = dx_p;
            depth = x;
        }
        const Float dy_m = std::abs( -sz_y - vy );
        if ( min_d > dy_m )
        {
            min_d = dy_m;
            depth = -y;
        }
        const Float dy_p = std::abs( sz_y - vy );
        if ( min_d > dy_p )
        {
            min_d = dy_p;
            depth = y;
        }
        depth = depth * min_d;

        // Now check if either vertices is insede and if closest face is the same face.
        if ( a.inside && (a.face_id == id) )
        {
            if ( a.depth < min_d )
                return false;
        }
        if ( b.inside && (b.face_id == id) )
        {
            if ( b.depth < min_d )
                return false;
        }

        // Fill in "at" and "depth".
        // At the moment "depth" should already be there.
        at = v_at - depth * 0.5;

        return true;
    }
};
struct Box
{
    BoxFace faces[6];
    BoxVertex verts[8];

    void init( const Vector3d sz2 )
    {
        // Init 6 faces.
        {
            BoxFace & v = faces[0];
            v.id = LEFT;
            v.n = Vector3d( -1.0, 0.0, 0.0 );
            v.center = Vector3d( -sz2.x_, 0.0, 0.0 );
            v.x = Vector3d( 0.0, 0.0, 1.0 );
            v.y = Vector3d( 0.0, 1.0, 0.0 );
            sz_x = sz2.z_;
            sz_y = sz2.y_;
        }
        {
            BoxFace & v = faces[1];
            v.id = BACK;
            v.n = Vector3d( 0.0, 0.0, 1.0 );
            v.center = Vector3d( 0.0, 0.0, sz2.z_ );
            v.x = Vector3d( 1.0, 0.0, 0.0 );
            v.y = Vector3d( 0.0, 1.0, 0.0 );
            sz_x = sz2.x_;
            sz_y = sz2.y_;
        }
        {
            BoxFace & v = faces[2];
            v.id = RIGHT;
            v.n = Vector3d( 1.0, 0.0, 0.0 );
            v.center = Vector3d( sz2.x_, 0.0, 0.0 );
            v.x = Vector3d( 0.0, 0.0, -1.0 );
            v.y = Vector3d( 0.0, 1.0, 0.0 );
            sz_x = sz2.z_;
            sz_y = sz2.y_;
        }
        {
            BoxFace & v = faces[3];
            v.id = FORWARD;
            v.n = Vector3d( 0.0, 0.0, -1.0 );
            v.center = Vector3d( 0.0, 0.0, -sz2.z_ );
            v.x = Vector3d( -1.0, 0.0, 0.0 );
            v.y = Vector3d( 0.0, 1.0, 0.0 );
            sz_x = sz2.x_;
            sz_y = sz2.y_;
        }
        {
            BoxFace & v = faces[4];
            v.id = DOWN;
            v.n = Vector3d( 0.0, -1.0, 0.0 );
            v.center = Vector3d( 0.0, -sz2.y, 0.0 );
            v.x = Vector3d( 1.0, 0.0, 0.0 );
            v.y = Vector3d( 0.0, 0.0, -1.0 );
            sz_x = sz2.x_;
            sz_y = sz2.z_;
        }
        {
            BoxFace & v = faces[5];
            v.id = UP;
            v.n = Vector3d( 0.0, 1.0, 0.0 );
            v.center = Vector3d( 0.0, sz2.y, 0.0 );
            v.x = Vector3d( 1.0, 0.0, 0.0 );
            v.y = Vector3d( 0.0, 0.0, -1.0 );
            sz_x = sz2.x_;
            sz_y = sz2.z_;
        }

        // Init 8 vertices.
        {
            const Vector3d vv[] = { Vector3d(-sz.x_, -sz.y_, -sz.z_), 
                                    Vector3d( sz.x_, -sz.y_, -sz.z_), 
                                    Vector3d( sz.x_,  sz.y_, -sz.z_ ), 
                                    Vector3d(-sz.x_,  sz.y_, -sz.z_), 
                                    Vector3d(-sz.x_, -sz.y_,  sz.z_), 
                                    Vector3d( sz.x_, -sz.y_,  sz.z_), 
                                    Vector3d( sz.x_,  sz.y_,  sz.z_ ), 
                                    Vector3d(-sz.x_,  sz.y_,  sz.z_) }; 
            for ( int i=0; i<8; i++ )
            {
                BoxVertex & v = verts[i];
                v.v = vv[i];
            }
        }
    }

    void apply( const Pose & pose )
    {
        for ( int i=0; i<6; i++ )
        {
            BoxFace & f = faces[i];
            f.n = pose.q * face.n;
            f.center = pose.r + (pose.q * f.center);
            f.x = pose.q * f.x;
            f.y = pose.q * f.y;
        }
        for ( int i=0; i<8; i++ )
        {
            BoxVertex & v = verts[i];
            v.v = pose.r + (pose.q * v.v);
        }
    }

    bool inside( CubeVertex & v ) const
    {
        FaceId min_id = BACK;
        Float min_depth = -1.0;
        for ( int i=0; i<6; i++ )
        {
            const BoxFace & f = faces[i];
            const Float nd = f.n.DotProduct( v.v - f.center );
            if ( nd > -EPS )
            {
                v.inside = false;
                v.depth = nd;
                return false;
            }
            const Float d = -nd;
            if ( (min_depth < 0.0) || (min_depth > d) )
            {
                min_id = f.id;
                min_depth = d;
            }
        }

        v.inside = true;
        v.face_id = min_id;
        v.depth = min_depth;
        return true;
    }
};

void collision_box_box( CollisionBox * obj_a, CollisionBox * obj_b, Vector<Vector3d> & ats, Vector<Vector3d> & depths )
{
    // Ok. Box A is converted to box B ref. frame.
    // Vertices of box A are tested if they are inside of box B.
    // It is saved towards what face the displacement is.
    // All edges of A are tested agains all faces of B.
    // In the case of intersection the smallest fix is computed.
    // If at the same time a vertex of this edge is inside the box B 
    // and its fix face is the same it is checked if fix distance is smaller.
    // If vertex distance is smaller, edge-face intersection is rejected.
    // If not smaller that iti s also added to toe intersections list.
    // Also, box need to be intersected symmetrically a->b and b->a. It naturally happens 
    // in collision processing loop.
    const Pose pose_a   = obj_a->pose_w();
    const Pose pose_b = obj_b->pose_w();
    const Pose pose_rel = pose_a / pose_b;
}











}







