
#include "pbd2_collision_utils.h"

namespace Pbd
{

static const Float EPS = 0.0001;

bool BoxFace::intersects( const BoxVertex & a, const BoxVertex & b, Vector3d & at, Vector3d & depth ) const
{
    const Vector3d b_a = b.v-a.v;
    const Float den = n.DotProduct( b_a );
    if ( std::abs(den) < EPS )
        return false;
    const Float num = n.DotProduct(center - a.v);
    const Float t = num / den;
    if ( (t < 0.0) || (t > 1.0) )
        return false;
    const Vector3d v_at = a.v + (b_a*t);
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

void Box::init( const Vector3d sz2 )
{
    // Init 6 faces.
    {
        BoxFace & v = faces[0];
        v.id = LEFT;
        v.n = Vector3d( -1.0, 0.0, 0.0 );
        v.center = Vector3d( -sz2.x_, 0.0, 0.0 );
        v.x = Vector3d( 0.0, 0.0, 1.0 );
        v.y = Vector3d( 0.0, 1.0, 0.0 );
        v.sz_x = sz2.z_;
        v.sz_y = sz2.y_;
    }
    {
        BoxFace & v = faces[1];
        v.id = BACK;
        v.n = Vector3d( 0.0, 0.0, 1.0 );
        v.center = Vector3d( 0.0, 0.0, sz2.z_ );
        v.x = Vector3d( 1.0, 0.0, 0.0 );
        v.y = Vector3d( 0.0, 1.0, 0.0 );
        v.sz_x = sz2.x_;
        v.sz_y = sz2.y_;
    }
    {
        BoxFace & v = faces[2];
        v.id = RIGHT;
        v.n = Vector3d( 1.0, 0.0, 0.0 );
        v.center = Vector3d( sz2.x_, 0.0, 0.0 );
        v.x = Vector3d( 0.0, 0.0, -1.0 );
        v.y = Vector3d( 0.0, 1.0, 0.0 );
        v.sz_x = sz2.z_;
        v.sz_y = sz2.y_;
    }
    {
        BoxFace & v = faces[3];
        v.id = FORWARD;
        v.n = Vector3d( 0.0, 0.0, -1.0 );
        v.center = Vector3d( 0.0, 0.0, -sz2.z_ );
        v.x = Vector3d( -1.0, 0.0, 0.0 );
        v.y = Vector3d( 0.0, 1.0, 0.0 );
        v.sz_x = sz2.x_;
        v.sz_y = sz2.y_;
    }
    {
        BoxFace & v = faces[4];
        v.id = DOWN;
        v.n = Vector3d( 0.0, -1.0, 0.0 );
        v.center = Vector3d( 0.0, -sz2.y_, 0.0 );
        v.x = Vector3d( 1.0, 0.0, 0.0 );
        v.y = Vector3d( 0.0, 0.0, -1.0 );
        v.sz_x = sz2.x_;
        v.sz_y = sz2.z_;
    }
    {
        BoxFace & v = faces[5];
        v.id = UP;
        v.n = Vector3d( 0.0, 1.0, 0.0 );
        v.center = Vector3d( 0.0, sz2.y_, 0.0 );
        v.x = Vector3d( 1.0, 0.0, 0.0 );
        v.y = Vector3d( 0.0, 0.0, -1.0 );
        v.sz_x = sz2.x_;
        v.sz_y = sz2.z_;
    }

    // Init 8 vertices.
    {
        const Vector3d vv[] = { Vector3d(-sz2.x_, -sz2.y_, -sz2.z_), 
                                Vector3d( sz2.x_, -sz2.y_, -sz2.z_), 
                                Vector3d( sz2.x_,  sz2.y_, -sz2.z_ ), 
                                Vector3d(-sz2.x_,  sz2.y_, -sz2.z_), 
                                Vector3d(-sz2.x_, -sz2.y_,  sz2.z_), 
                                Vector3d( sz2.x_, -sz2.y_,  sz2.z_), 
                                Vector3d( sz2.x_,  sz2.y_,  sz2.z_ ), 
                                Vector3d(-sz2.x_,  sz2.y_,  sz2.z_) }; 
        for ( int i=0; i<8; i++ )
        {
            BoxVertex & v = verts[i];
            v.v = vv[i];
        }
    }
}

void Box::apply( const Pose & pose )
{
    for ( int i=0; i<6; i++ )
    {
        BoxFace & f = faces[i];
        f.n = pose.q * f.n;
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

bool Box::inside( BoxVertex & v ) const
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

bool Box::inside_const( const BoxVertex & v ) const
{
    for ( int i=0; i<6; i++ )
    {
        const BoxFace & f = faces[i];
        const Float nd = f.n.DotProduct( v.v - f.center );
        if ( nd > -EPS )
            return false;
    }

    return true;
}

bool Box::intersects( const Box & b ) const
{
    for ( int i=0; i<8; i++ )
    {
        const BoxVertex & v = b.verts[i];
        const bool ok = inside_const( v );
        if ( ok )
            return true;
    }

    static const int edges[12][2] = { {0, 1}, {1, 2}, {2, 3}, {3, 0}, 
                                      {4, 5}, {5, 6}, {6, 7}, {7, 4}, 
                                      {0, 4}, {1, 5}, {2, 6}, {3, 7} };
    for ( int i=0; i<12; i++ )
    {
        const int ind_a = edges[i][0];
        const int ind_b = edges[i][1];
        const BoxVertex & va = b.verts[ind_a];
        const BoxVertex & vb = b.verts[ind_b];
        for ( int j=0; j<6; j++ )
        {
            const BoxFace & f = faces[j];
            Vector3d at, depth;
            const bool intersects = f.intersects( va, vb, at, depth );
            if ( intersects )
                return true;
        }
    }

    return false;
}





}













