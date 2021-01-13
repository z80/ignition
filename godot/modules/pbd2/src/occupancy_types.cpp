
#include "occupancy_types.h"


namespace Pbd
{

static const Float EPS = 0.000001;

bool common_perpendicular( const Vector3d & ra0, const Vector3d & ra1, const Vector3d & rb0, const Vector3d & rb1, Vector3d & ra, Vector3d & rb )
{
    {
        const Vector3d a1 = ra1 - ra0;
        const Vector3d a2 = rb1 - rb0;
        const Vector3d n  = a1.CrossProduct( a1.CrossProduct(a2) );
        const Float den = a2.DotProduct( n );
        if ( std::abs(den) < EPS )
            return false;

        const Float num = n.DotProduct(ra0 - rb0);
        const Float t = num / den;
        // This point is on the "f" edge.
        rb = rb0 + a2*t;
    }

    {
        const Vector3d a1 = rb1 - rb0;
        const Vector3d a2 = ra1 - ra0;
        const Vector3d n  = a1.CrossProduct( a1.CrossProduct(a2) );
        const Float den = a2.DotProduct( n );
        if ( std::abs(den) < EPS )
            return false;

        const Float num = n.DotProduct(rb0 - ra0);
        const Float t = num / den;
        // This point is on the "f" edge.
        ra = ra0 + a2*t;
    }

    return true;
}


void Plane::init( const Vector3d & a, const Vector3d & b, const Vector3d & c )
{
    const Vector3d r1 = b - a;
    const Vector3d r2 = c - a;
    norm = r1.CrossProduct( r2 );
    norm.Normalize();
    d = -a.DotProduct( norm );

        r0 = a;
}

bool Plane::above( const Vector3d & r ) const
{
    const Float h = norm.DotProduct(r) + d;
    const bool ret = (h > 0.0);
    return ret;
}

bool Plane::intersects( const Vector3d & a, const Vector3d & b, Vector3d & at ) const
{
    const Float den = norm.DotProduct(b - a);
    const Float abs_den = std::abs( den );
    if ( abs_den < EPS )
        return false;

    const Float num = norm.DotProduct( a ) + d;
    const Float t = -num/den;
    if ( (t < 0.0) || (t > 1.0) )
        return false;

    at = a + (b - a)*t;
    return true;
}








void Face::init( const Vector3d & a, const Vector3d & b, const Vector3d & c )
{
    verts_0[0] = a;
    verts_0[1] = b;
    verts_0[2] = c;
    
    init();
}


void Face::init()
{
    for ( int i=0; i<3; i++ )
    {
            const Vector3d & v0 = verts_0[i];
            Vector3d & v = verts[i];
            v = v0;
    }
    init_planes();
}


void Face::apply( const SE3 & se3 )
{
    for ( int i=0; i<3; i++ )
    {
        const Vector3d & v0 = verts_0[i];
        Vector3d & v = verts[i];
        v = se3.r_ + se3.q_ * v0;
    }
    init_planes();
}


void Face::init_planes()
{
    plane.init( verts[0], verts[1], verts[2] );
    planes[0].init( verts[0], verts[1], verts[0]+plane.norm );
    planes[1].init( verts[1], verts[2], verts[1]+plane.norm );
    planes[2].init( verts[2], verts[0], verts[2]+plane.norm );
}

bool Face::intersects( const Vector3d & r1, const Vector3d & r2, Vector3d & at ) const
{
    const bool intersects_plane = plane.intersects( r1, r2, at );
    if ( !intersects_plane )
        return false;

    for ( int i=0; i<3; i++ )
    {
        const Plane & pl = planes[i];
        const bool above_a = pl.above( at );
        if ( above_a )
            return false;
    }

    return true;
}

bool Face::intersects( const Face & f, Vector3d & at, Vector3d & depth ) const
{
    // Check each edge.
    static const int edges[3][2] = { {0, 1}, {1, 2}, {2, 0} };
    int intersecting_a_edge_index = -1;
    Vector3d at_a, head_a;
    for ( int i=0; i<3; i++ )
    {
        const int ind_a = edges[i][0];
        const int ind_b = edges[i][1];
        const Vector3d & a = verts[ind_a];
        const Vector3d & b = verts[ind_b];
        const bool ok = f.intersects( a, b, at_a );
        if ( !ok )
            continue;
        // Remember intersecting edge index.
        intersecting_a_edge_index = i;
        // Determine which point is undernear.
        const Vector3d ba = b - a;
        const Float dir = ba.DotProduct( f.plane.norm );
        const bool head_inside = ( dir < 0.0 );
        // "head" inside, "tail" outside.
        const Vector3d tail = (head_inside) ? a : b;
        head_a = (head_inside) ? b : a;
        // Need to crop head within side plane bounds.
        for ( int j=0; j<3; j++ )
        {
            const Plane & pl = f.planes[j];
            Vector3d cropped_head;
            const bool intersects = pl.intersects( tail, head_a, cropped_head );
            if ( intersects )
            {
                head_a = cropped_head;
                break;
            }
        }
    }

    // Intersect the other way around.
    int intersecting_b_edge_index = -1;
    Vector3d at_b, head_b;
    for ( int i=0; i<3; i++ )
    {
        const int ind_a = edges[i][0];
        const int ind_b = edges[i][1];
        const Vector3d & a = f.verts[ind_a];
        const Vector3d & b = f.verts[ind_b];
        const bool ok = intersects( a, b, at_b );
        if ( !ok )
            continue;
        // Remember intersecting edge index.
        intersecting_b_edge_index = i;
        // Determine which point is undernear.
        const Vector3d ba = b - a;
        const Float dir = ba.DotProduct( plane.norm );
        const bool head_inside = ( dir < 0.0 );
        // "head" inside, "tail" outside.
        const Vector3d tail = (head_inside) ? a : b;
        head_b = (head_inside) ? b : a;
        // Need to crop head within side plane bounds.
        for ( int j=0; j<3; j++ )
        {
            const Plane & pl = planes[j];
            Vector3d cropped_head;
            const bool intersects = pl.intersects( tail, head_b, cropped_head );
            if ( intersects )
            {
                head_b = cropped_head;
                break;
            }
        }
    }
    
    const bool intersects_a = (intersecting_a_edge_index >= 0);
    const bool intersects_b = (intersecting_b_edge_index >= 0);
    if ( intersects_a && intersects_b )
    {
        // Use common perpendicular.
        // Own points.
        const int ind_a_0 = edges[intersecting_a_edge_index][0];
        const int ind_a_1 = edges[intersecting_a_edge_index][1];
        const Vector3d & ra0 = verts[ind_a_0];
        const Vector3d & ra1 = verts[ind_a_1];
        // Other points.
        const int ind_b_0 = edges[intersecting_b_edge_index][0];
        const int ind_b_1 = edges[intersecting_b_edge_index][1];
        const Vector3d & rb0 = f.verts[ind_b_0];
        const Vector3d & rb1 = f.verts[ind_b_1];
        // Solving for the common perpendicular between the two crossing lines.
        Vector3d a, b;
        const bool ok = common_perpendicular( ra0, ra1, rb0, rb1, a, b );
        if ( !ok )
            return false;
        
        at    = (a + b) * 0.5;
        depth = (a - b);

        return true;
    }
    else if ( intersects_a )
    {
        // This edges intersect "f" plane.
        const Vector3d d = at_a - head_a;
        depth = f.plane.norm * f.plane.norm.DotProduct( d );
        at    = head_a;
        return true;
    }
    else if ( intersects_b )
    {
        // This edges intersect "f" plane.
        const Vector3d d = at_b - head_b;
        depth = plane.norm * plane.norm.DotProduct( d );
        at    = head_b;
        return true;
    }

    return false;
}

struct Collision
{
        // Which edge (0, 1, 2) intersects or is the closest.
        int edge_this;
        // Which edge (0, 1, 2) in the other is the closest or intersects.
        int edge_other;
        // Intersection point.
        Vector3d at;
        // Correction displacement to fix penetration. "this" moves, "other" stays in place.
        Vector3d depth;
};

int Face::intersects_all( const Face & f, Vector3d * ats, Vector3d * depths ) const
{
        // Check each edge.
        static const int edge_verts[3][2] = { {0, 1}, {1, 2}, {2, 0} };
        static const int vert_edges[3][2] = { {0, 2}, {0, 1}, {1, 2} };

        Collision collisions[2];
        int collisions_qty = 0;

        // Check if edges of "*this" intersect "f".
        for ( int i=0; i<3; i++ )
        {
                const int ind_a_0 = edge_verts[i][0];
                const int ind_a_1 = edge_verts[i][1];
                const Vector3d & a = verts[ind_a_0];
                const Vector3d & b = verts[ind_a_1];
                Vector3d at;
                const bool ok = f.intersects( a, b, at );
                if (!ok)
                        continue;
                // Check which vertex is under the surface.
                const bool a_is_above = f.plane.above( a );
                const Vector3d head = (a_is_above) ? b : a;
                // Compute the depth.
                const Vector3d displacement_raw = at - head;
                // Project it on the normal.
                const Vector3d displacement_point = f.plane.norm * ( displacement_raw.DotProduct( f.plane.norm ) );
                const Float displacement_point_depth = displacement_point.Length();

                // Now search for the common perpendicualr.
                Vector3d common_perp_displacement;
                Float common_perp_depth = -1.0;
                int common_perp_index = -1;
                for ( int j=0; j<3; j++ )
                {
                        const int ind_b_0 = edge_verts[j][0];
                        const int ind_b_1 = edge_verts[j][1];
                        const Vector3d & c = f.verts[ind_b_0];
                        const Vector3d & d = f.verts[ind_b_1];

                        Vector3d ra, rb;
                        const bool ok = common_perpendicular( a, b, c, d, ra, rb );
                        if (!ok)
                                continue;

                        Vector3d disp = ra - rb;
                        const Float depth = disp.Length();
                        if ( (common_perp_index < 0) || (common_perp_depth > depth) )
                        {
                                common_perp_index = j;
                                common_perp_depth = depth;
                                const Float sign = disp.DotProduct( f.plane.norm );
                                if ( sign > 0.0 )
                                        disp = -disp;
                                common_perp_displacement = disp;
                        }
                }

                Collision & c = collisions[collisions_qty];
                c.edge_this  = i;
                c.edge_other = common_perp_index;
                c.at         = at;
                        // Now compare the point depth and common perpendicular.
                if ( displacement_point_depth <= common_perp_depth )
                        c.depth = displacement_point;
                else
                        c.depth = common_perp_displacement;
                collisions_qty += 1;
        }

        // If already 2 points, return the points.
        if ( collisions_qty >= 2 )
        {
                for ( int i=0; i<collisions_qty; i++ )
                {
                        const Collision & c = collisions[i];
                        ats[i]    = c.at;
                        depths[i] = c.depth;
                }
                return collisions_qty;
        }
        // Now do the exactly opposite thing.
        // Check "f"'s edges for collision with "*this"'s plane.
        for ( int i=0; i<3; i++ )
        {
                const int ind_a_0 = edge_verts[i][0];
                const int ind_a_1 = edge_verts[i][1];
                const Vector3d & a = f.verts[ind_a_0];
                const Vector3d & b = f.verts[ind_a_1];

                Vector3d at;
                const bool ok = intersects( a, b, at );
                if ( !ok )
                        continue;

                // Check which vertex is under the surface.
                const bool a_is_above = plane.above( a );
                const Vector3d head = (a_is_above) ? b : a;
                // Compute the depth.
                const Vector3d displacement_raw = at - head;
                // Project it on the normal.
                const Vector3d displacement_point = -plane.norm * ( displacement_raw.DotProduct( plane.norm ) );
                const Float displacement_point_depth = displacement_point.Length();

                // Now search for the common perpendicualr.
                Vector3d common_perp_displacement;
                Float common_perp_depth = -1.0;
                int common_perp_index = -1;
                for ( int j=0; j<3; j++ )
                {
                        const int ind_b_0 = edge_verts[j][0];
                        const int ind_b_1 = edge_verts[j][1];
                        const Vector3d & c = verts[ind_b_0];
                        const Vector3d & d = verts[ind_b_1];

                        Vector3d ra, rb;
                        const bool ok = common_perpendicular( a, b, c, d, ra, rb );
                        if (!ok)
                                continue;

                        Vector3d disp = ra - rb;
                        const Float depth = disp.Length();
                        if ( (common_perp_index < 0) || (common_perp_depth > depth) )
                        {
                                common_perp_index = j;
                                common_perp_depth = depth;
                                const Float sign = disp.DotProduct( plane.norm );
                                if ( sign > 0.0 )
                                        disp = -disp;
                                common_perp_displacement = disp;
                        }
                }

                Collision & c = collisions[collisions_qty];
                c.edge_this  = i;
                c.edge_other = common_perp_index;
                c.at         = at;
                // Now compare the point depth and common perpendicular.
                if ( displacement_point_depth <= common_perp_depth )
                        c.depth = displacement_point;
                else
                        c.depth = common_perp_displacement;
                collisions_qty += 1;
        }

        // Return collisions.
        for ( int i=0; i<collisions_qty; i++ )
        {
                const Collision & c = collisions[i];
                ats[i]    = c.at;
                depths[i] = c.depth;
        }
        return collisions_qty;
}








bool Face::intersects_2( const Face & f, Vector3d * at, Vector3d * depth ) const
{
    // Constants defining vertices and edges in a triangle.
    static const int edge_verts[3][2] = { {0, 1}, {1, 2}, {2, 0} };
    static const int vert_edges[3][2] = { {0, 2}, {0, 1}, {1, 2} };

    // Check if triangles intersect at all.
    // It is done by determining if edge vertices are on the opposite sides of the 
    // other triangle plane.
    bool vert_a_above[3];
    bool vert_b_above[3];
    {
        int below_a_qty;
        int below_b_qty;
        for ( int i=0; i<3; i++ )
        {
            const bool above_a = f.plane.above( verts[i] );
            vert_a_above[i] = above_a;
            if ( !above_a )
                below_a_qty += 1;

            const bool above_b = plane.above( f.verts[i] );
            vert_b_above[i] = above_b;
            if ( !above_b )
                below_b_qty += 1;
        }
        
        // If triangle is fully on one side, no intersection.
        if ( (below_a_qty == 0) || (below_a_qty == 3) )
            return false;
        if ( (below_b_qty == 0) || (below_b_qty == 3) )
            return false;
    }


    // 1) Search for a common line.
    Vector3d common_a = plane.norm.CrossProduct( f.plane.norm );
    {
        const Float common_a_length = common_a.Length();
        // If too short, means, triangles are parallel.
        // No collision in this case.
        if ( common_a_length < EPS )
            return false;
        common_a = common_a / common_a_length;
    }

    // Searching for a point on the line.
    // This point satisfies to both planes simultaneously.
    Vector3d common_r;
    {
        const Vector3d r0 = verts[0];
        const Vector3d a = verts[1] - verts[0];
        const Vector3d b = verts[2] - verts[0];
        // a*ta + b*tb + r0 - all points within the triangle's plane.
        // Intersect it with the other triangle and require it to satisfy it.
        // (a, f.plane.n)*ta + (b, f.plane.n)*tb + (r0, f.plane.n) + f.plane.d = 0
        const Float a_n = a.DotProduct( f.plane.norm );
        const Float b_n = b.DotProduct( f.plane.norm );
        const Float rhs = -( f.plane.d + r0.DotProduct( f.plane.norm ) );
        // Either "a" or "b" might be parallel to other triangle plane. So 
        // check which one is less parallel and use it in the denominator.
        Float ta, tb;
        if ( std::abs(a_n) > std::abd(b_n) )
        {
            tb = 0.0;
            ta = rhs / a_n;
        }
        else
        {
            ta = 0.0;
            tb = ths / b_n;
        }
        common_r = r0 + a*ta + b*tb;
    }

    // 2) Compute line parameter number "t" in the equation "r = common_a*t + common_r" 
    // which corresponds intersection with edge planes of each triangle.
    Float    t_a[2];
    Vector3d at_a[2];
    int      edge_ind_a[0];
    Float    t_b[2];
    Vector3d at_b[2];
    int      edge_ind_b[2];
    int qty_a = 0;
    int qty_b = 0;
    for ( int i=0; i<3; i++ )
    {
        const int ind_0 = edge_verts[i][0];
        const int ind_1 = edge_verts[i][1];

        // First "this" thiangle.
        const bool above_a_0 = vert_a_above[ind_0];
        const bool above_a_1 = vert_a_above[ind_1];
        if ( above_a_0 ^ above_a_1 )
        {
            const Plane & pl = planes[i];
            const Float den = common_a.DotProduct( pl.norm );
            if ( std::abs(den) < EPS )
                return false;
            const Float num = common_r.DotProduct( pl.norm ) + pl.d;
            const Float t = -num/den;
            const Vector3d at = common_a*t + common_r;
            t_a[qty_a] = t;
            at_a[qty_a] = at;
            edge_ind_a[qty_a] = i;
            qty_a += 1;
        }

        // And now "f" triangle.
        const bool above_b_0 = vert_b_above[ind_0];
        const bool above_b_1 = vert_b_above[ind_1];
        if ( above_b_0 && (!above_b_1) )
        {
            const Plane & pl = f.planes[i];
            const Float den = common_b.DotProduct( pl.norm );
            if ( std::abs(den) < EPS )
                return false;
            const Float num = common_r.DotProduct( pl.norm ) + pl.d;
            const Float t = -num/den;
            const Vector3d at = common_a*t + common_r;
            t_b[qty_b] = t;
            at_b[qty_b] = at;
            edge_ind_b[qty_b] = i;
            qty_b += 1;
        }

    }

    // Sort points in accending "t" order.
    if ( t_a[0] > t_a[1] )
    {
        const int t = t_a[0];
        t_a[0] = t_a[1];
        t_a[1] = t;
        
        const Vector3d at = at_a[0];
        at_a[0] = at_a[1];
        at_a[1] = at;

        const int edge_ind = edge_ind_a[0];
        edge_ind_a[0] = edge_ind_a[1];
        edge_ind_a[1] = edge_ind;
    }
    if ( t_b[0] > t_b[1] )
    {
        const int t = t_b[0];
        t_b[0] = t_b[1];
        t_b[1] = t;
        
        const Vector3d at = at_b[0];
        at_b[0] = at_b[1];
        at_b[1] = at;

        const int edge_ind = edge_ind_b[0];
        edge_ind_b[0] = edge_ind_b[1];
        edge_ind_b[1] = edge_ind;
    }
    
    // They don't intersect id smallest "t" for one interval is 
    // begger than biggest "t" for the other interval.
    if ( (t_a[0] > t_b[1]) || (t_a[1] < t_b[0]) )
        return false;

    // At this point intervals do intersect.
    // But they either partially overlap 
    // or one fully contains the other.
    // Regardless of that pick the biggest of the two left bounds 
    // and the smallest of the two right bounds.
    Vector3d depth_0;
    {
        const bool pick_a_0  = ( t_a[0] > t_b[0] );
        const Vector3d at_0  = (pick_a_0) ? at_a[0] : at_b[0];
        const int edge_ind_0 = (pick_a_0) ? edge_ind_a[0] : edge_ind_b[0];

        // First edge crossing depth.
        Vector3d head;
        Face * crossing;
        Face * crossed; // The triangle being crossed
        const int ind_0 = edge_verts[edge_ind_0][0];
        const int ind_1 = edge_verts[edge_ind_0][1];
        if ( pick_a_0 )
        {
            const bool ind_0_above = vert_a_above[ind_0];
            head = (ind_0_above) ? verts[ind_1] : verts[ind_0];
            crossing = this;
            crossed  = &f;
        }
        else
        {
            const bool ind_0_above = vert_b_above[ind_0];
            head = (ind_0_above) ? f.verts[ind_1] : f.verts[ind_0];
            crossing = &f;
            crossed  = this;
        }
        // Surround (at->head) vector by face side planes and limit head.
        for ( int i=0; i<3; i++ )
        {
            const Plane & pl = crossed->planes[i];
            Vector3d cropped_head;
            const bool intersects_ok = pl.intersects( at_0, head, cropped_head );
            if ( intersects_ok )
            {
                head = cropped_head;
                break;
            }
        }
        // Project the vector onto "crossed" normal.
        Vector3d crossing_depth = head - at_0;
        crossing_depth = crossed->plane.norm * ( crossed->plane.norm.DotProduct( crossing_depth ) );
        // Crossing depth should be in the same direction as "f"'s normal.
        const Float along_f = crossing_depth.DotProduct( f.plane.norm );
        if ( along_f < 0.0 )
            crossing_depth = -crossing_depth;
        depth_0 = crossing_depth;

        // Crossed triangle depth directly undernear the point.
        // It's the intersection of appropriate crossing edge's plane with 
        // all the edges of crossed triangle edges. And taking the one below 
        // the plane of crossed triangle.
        const Plane & crossing_plane = crossing->planes[edge_ind_0];
        Vector3d crossed_depth;
        for ( int i=0; i<3; i++ )
        {
            const int vert_ind_0 = edge_verts[i][0];
            const int vert_ind_1 = edge_verts[i][1];
            const Vector3d & r0 = crossed->verts[vert_ind_0];
            const Vector3d & r1 = crossed->verts[vert_ind_b];
            const Vector3d a = r1 - r0;
            const Float den = a.DotProduct( crossing_plane.norm );
            if ( std::abs(den) < EPS )
                continue;
            const Float num = r0.DotProduct( crossing_plane.norm ) + crossing_plane.d;
            const Float t = -(num/den);
            if ( ( t >= 0.0 ) || ( t <= 1.0 ) )
                continue;
            const Vector3d pt = r0 + a*t;
            // Check if it is under the crossed triangle plane.
            const Vector3d dr = pt - crossed->verts[0];
            const Float dr_is_above = dr.DotProduct( crossed->plane.norm );
            if ( dr_is_above > 0.0 )
                continue;

            crossed_depth = pt - at_0;
            // It should be in the same direction as f.plane.norm;
            const Float along_f = crossed_depth.DotProduct( f.plane.norm );
            if ( along_f < 0.0 )
                crossed_depth = -crossed_depth;
            // Choose the smallest depth.
            const Float depth_0_lenght       = depth_0.LengthSquared();
            const Float crossed_depth_length = crossed_depth.LengthSquared();
            if ( depth_0_lenght > crossed_depth_length )
                depth_0 = crossed_depth;
            // If found the crossing point below, interrupt the loop, 
            // as there should be only one point intersecting triangle below.
            break;
        }

        // Crossed triangle depth directly behind the point.
        // Don't implement it just yet. Even though this option is possible, 
        // but with continuous bodies motion previous two should be enough.
        // (At least I think so at the moment).
    }



    Vector3d depth_1;
    {
        const bool pick_a_1 = ( t_a[1] < t_b[1] );
        const Vector3d at_1  = (pick_a_1) ? at_a[1] : at_b[1];
        const int edge_ind_1 = (pick_a_1) ? edge_ind_a[1] : edge_ind_b[1];

        // First edge crossing depth.
        Vector3d head;
        Face * crossing;
        Face * crossed; // The triangle being crossed.
        const int ind_0 = edge_verts[edge_ind_1][0];
        const int ind_1 = edge_verts[edge_ind_1][1];
        if ( pick_a_1 )
        {
            const bool ind_0_above = vert_a_above[ind_0];
            head = (ind_0_above) ? verts[ind_1] : verts[ind_0];
            crossing = this;
            crossed  = &f;
        }
        else
        {
            const bool ind_0_above = vert_b_above[ind_0];
            head = (ind_0_above) ? f.verts[ind_1] : f.verts[ind_0];
            crossing = &f;
            crossed  = this;
        }
        // Surround (at->head) vector by face side planes and limit head.
        for ( int i=0; i<3; i++ )
        {
            const Plane & pl = crossed->planes[i];
            Vector3d cropped_head;
            const bool intersects_ok = pl.intersects( at_1, head, cropped_head );
            if ( intersects_ok )
            {
                head = cropped_head;
                break;
            }
        }
        // Project the vector onto "crossed" normal.
        Vector3d crossing_depth = head - at_1;
        crossing_depth = crossed->plane.norm * ( crossed->plane.norm.DotProduct( crossing_depth ) );
        // Crossing depth should be in the same direction as "f"'s normal.
        const Float along_f = crossing_depth.DotProduct( f.plane.norm );
        if ( along_f < 0.0 )
            crossing_depth = -crossing_depth;
        depth_1 = crossing_depth;
    

        // Crossed triangle depth directly undernear the point.
        // It's the intersection of appropriate crossing edge's plane with 
        // all the edges of crossed triangle edges. And taking the one below 
        // the plane of crossed triangle.
        const Plane & crossing_plane = crossing->planes[edge_ind_0];
        for ( int i=0; i<3; i++ )
        {
            const int vert_ind_0 = edge_verts[i][0];
            const int vert_ind_1 = edge_verts[i][1];
            const Vector3d & r0 = crossed->verts[vert_ind_0];
            const Vector3d & r1 = crossed->verts[vert_ind_b];
            const Vector3d a = r1 - r0;
            const Float den = a.DotProduct( crossing_plane.norm );
            if ( std::abs(den) < EPS )
                continue;
            const Float num = r0.DotProduct( crossing_plane.norm ) + crossing_plane.d;
            const Float t = -(num/den);
            if ( ( t >= 0.0 ) || ( t <= 1.0 ) )
                continue;
            const Vector3d pt = r0 + a*t;
            // Check if it is under the crossed triangle plane.
            const Vector3d dr = pt - crossed->verts[0];
            const Float dr_is_above = dr.DotProduct( crossed->plane.norm );
            if ( dr_is_above > 0.0 )
                continue;

            Vector3d crossed_depth = pt - at_1;
            // It should be in the same direction as f.plane.norm;
            const Float along_f = crossed_depth.DotProduct( f.plane.norm );
            if ( along_f < 0.0 )
                crossed_depth = -crossed_depth;
            // Choose the smallest depth.
            const Float depth_1_lenght = depth_1.LengthSquared();
            const Float crossed_depth_length  = crossed_depth.LengthSquared();
            if ( depth_1_lenght > crossed_depth_length )
                depth_1 = crossed_depth;
            // If found the crossing point below, interrupt the loop, 
            // as there should be only one point intersecting triangle below.
            break;
        }

        // Crossed triangle depth directly behind the point.
        // Don't implement it just yet. Even though this option is possible, 
        // but with continuous bodies motion previous two should be enough.
        // (At least I think so at the moment).
    }
    
    // Fill in the results. 
    ats[0] = at_0;
    ats[1] = at_1;
    depths[0] = depth_0;
    depths[1] = depth_1;

    // Return success.
    return true;
}


















void Cube::init( const Vector3d & c, Float x2, Float y2, Float z2 )
{
        center_0 = c;
    center = center_0;
    szx2 = std::abs( x2 );
    szy2 = std::abs( y2 );
    szz2 = std::abs( z2 );
    ex = Vector3d( 1.0, 0.0, 0.0 );
    ey = Vector3d( 0.0, 1.0, 0.0 );
    ez = Vector3d( 0.0, 0.0, 1.0 );

    init_verts_and_planes();

    // Also init AABB size.
    aabb2 = szx2;
    if ( aabb2 < szy2 )
        aabb2 = szy2;
    if (aabb2 < szz2)
        aabb2 = szz2;
    // Multiply by sqrt(3);
    aabb2 *= 1.7320508075688772935274463415059;
}


void Cube::apply( const SE3 & se3 )
{
    center = se3.r_ + se3.q_ * center_0;
    ex = Vector3d( 1.0, 0.0, 0.0 );
    ey = Vector3d( 0.0, 1.0, 0.0 );
    ez = Vector3d( 0.0, 0.0, 1.0 );
    ex = se3.q_ * ex;
    ey = se3.q_ * ey;
    ez = se3.q_ * ez;

    init_verts_and_planes();

    // Also init AABB size.
    aabb2 = szx2;
    if ( aabb2 < szy2 )
        aabb2 = szy2;
    if (aabb2 < szz2)
        aabb2 = szz2;
    // Multiply by sqrt(3);
    aabb2 *= 1.7320508075688772935274463415059;
}


void Cube::init_verts_and_planes()
{
    // Vertices.
    verts[0] = center - szx2*ex - szy2*ey - szz2*ez;
    verts[1] = center + szx2*ex - szy2*ey - szz2*ez;
    verts[2] = center + szx2*ex + szy2*ey - szz2*ez;
    verts[3] = center - szx2*ex + szy2*ey - szz2*ez;


    verts[4] = center - szx2*ex - szy2*ey + szz2*ez;
    verts[5] = center + szx2*ex - szy2*ey + szz2*ez;
    verts[6] = center + szx2*ex + szy2*ey + szz2*ez;
    verts[7] = center - szx2*ex + szy2*ey + szz2*ez;

    // Outward planes.
    // Down.
    planes[0].init( verts[0], verts[3], verts[1] );
    // Up.
    planes[1].init( verts[4], verts[5], verts[7] );
    // Left.
    planes[2].init( verts[0], verts[4], verts[3] );
    // Back.
    planes[3].init( verts[0], verts[1], verts[4] );
    // Right.
    planes[4].init( verts[1], verts[2], verts[5] );
    // Forward.
    planes[5].init( verts[2], verts[3], verts[6] );
}


bool Cube::intersects( const Vector3d & a, const Vector3d & b ) const
{
    bool above_a[6];
    bool above_b[6];
    int above_a_qty = 0;
    int above_b_qty = 0;
    for ( int i=0; i<6; i++ )
    {
        const bool aa = planes[i].above( a );
        const bool ab = planes[i].above( b );
        // If both points are above plane, means 
        // both points are outside the cube.
        if ( aa && ab )
            return false;
        above_a[i] = aa;
        above_b[i] = ab;
        if ( aa )
            above_a_qty += 1;
        if ( ab )
            above_b_qty += 1;
    }
    // If either of points is under all planes, 
    // means it is inside the cube.
    if ( above_a_qty == 0 )
        return true;
    if ( above_b_qty == 0 )
        return true;
    
    // The case when the line intersects the cube but 
    // both points are outside.
    // Intersect the line with each plane.
    // And determine if collision point is within face bounds.
    for ( int i=0; i<6; i++ )
    {
        const bool aa = above_a[i];
        const bool ab = above_b[i];
        if ( !(aa ^ ab) )
            continue;
        
        const Plane & plane = planes[i];
        const Float den = plane.norm.DotProduct(b-a);
        // If it is parallel to the face, denominator is zero.
        const bool is_ok = ( std::abs(den) > EPS );
        if ( !is_ok )
            continue;
        // Restore collision point.
        const Float num = plane.d + a.DotProduct( plane.norm );
        const Float t = -num/den;
        // Intersection point relative to the center.
        const Vector3d at = a + t*(b-a) - center;
        const Float dist_x = std::abs( at.DotProduct( ex ) );
        const Float dist_y = std::abs( at.DotProduct( ey ) );
        const Float dist_z = std::abs( at.DotProduct( ez ) );
        // At least two distances should be smaller than appropriate cube dimensions.
        int qty = 0;
        if ( dist_x < szx2 )
            qty += 1;
        if ( dist_y < szy2 )
            qty += 1;
        if ( dist_z < szz2 )
            qty += 1;
        if ( qty > 1 )
            return true;
    }

    return false;
}


bool Cube::intersects( const Cube & c ) const
{
    // Check if all vertices are outside of either of 6 planes.
    for ( int i=0; i<6; i++ )
    {
        const Plane & plane = planes[i];
        int out_qty = 0;
        for ( int j=0; j<8; j++ )
        {
            const Vector3d & pt = c.verts[j];
            const bool point_is_above = plane.above( pt );
            if ( point_is_above )
                out_qty += 1;
                        else
                                break;
        }
        if ( out_qty > 7 )
            return false;
    }
    // Now check the other way.
    for ( int i=0; i<6; i++ )
    {
        const Plane & plane = c.planes[i];
        int out_qty = 0;
        for ( int j=0; j<8; j++ )
        {
            const Vector3d & pt = verts[j];
            const bool point_is_above = plane.above( pt );
            if ( point_is_above )
                out_qty += 1;
                        else
                                break;
        }
        if ( out_qty > 7 )
            return false;
    }

    // Now check if a vertex is undernear all the planes.
    for ( int i=0; i<8; i++ )
    {
        const Vector3d & pt = verts[i];
        int out_qty = 0;
        for ( int j=0; j<6; j++ )
        {
            const Plane & plane = c.planes[j];
            const bool above = plane.above( pt );
            if ( above )
            {
                out_qty += 1;
                break;
            }
        }
        if ( out_qty > 0 )
            continue;
        return true;
    }
    // Check other cube vertices.
    for ( int i=0; i<8; i++ )
    {
        const Vector3d & pt = c.verts[i];
        int out_qty = 0;
        for ( int j=0; j<6; j++ )
        {
            const Plane & plane = planes[j];
            const bool above = plane.above( pt );
            if ( above )
            {
                out_qty += 1;
                break;
            }
        }
        if ( out_qty > 0 )
            continue;
        return true;
    }

    // At this point none of the vertices inside of the other cube.
    // Check edge intersections
    static const int edges[12][2] = { {0, 1}, {1, 2}, {2, 3}, {3, 0}, 
                                      {0, 4}, {1, 5}, {2, 6}, {3, 7}, 
                                      {4, 5}, {5, 6}, {6, 7}, {7, 4} };
    for ( int i=0; i<12; i++ )
    {
        const int ind_a = edges[i][0];
        const int ind_b = edges[i][1];
        const bool intersects_ = intersects( c.verts[ind_a], c.verts[ind_b] );
        if ( intersects_ )
            return true;
    }
    // And the other way around.
    for ( int i=0; i<12; i++ )
    {
        const int ind_a = edges[i][0];
        const int ind_b = edges[i][1];
        const bool intersects_ = c.intersects( verts[ind_a], verts[ind_b] );
        if ( intersects_ )
            return true;
    }

    return false;
}



bool Cube::intersects( const Face & f ) const
{
    // Check if all cube vertices are on one side of triangle plane.
    int qty = 0;
    for (int i=0; i<8; i++)
    {
        const Vector3d & v = verts[i];
        const bool above = f.plane.above( v );
        if ( above )
            qty += 1;
        else
            qty -= 1;
    }
    if ( qty > 7 )
        return false;
    else if ( qty < -7 )
        return false;

    // Check if all triangle vertices are outside of either cube plane.
    for ( int i=0; i<6; i++ )
    {
        const Plane & pl = planes[i];
        qty = 0;
        for ( int j=0; j<3; j++ )
        {
            const Vector3d & v = f.verts[j];
            const bool out = pl.above( v );
            if ( out )
                qty += 1;
                        else
                                break;
        }
        if ( qty > 2 )
            return false;
    }

    // Check if either triangle vertex is inside of a cube.
    for ( int i=0; i<3; i++ )
    {
        const Vector3d & v = f.verts[i];
        qty = 0;
        for ( int j=0; j<6; j++ )
        {
            const Plane & pl = planes[j];
            const bool outside = pl.above( v );
            if ( !outside )
                qty += 1;
        }
        if ( qty > 5 )
            return true;
    }

    // The only option left is if cube and triangle intersects.
    // Compute if triangle edges intersect cube.
    for ( int i=0; i<3; i++ )
    {
        const int ind_a = i;
        const int ind_b = (i+1) % 3;
        const Vector3d & a = f.verts[ind_a];
        const Vector3d & b = f.verts[ind_b];
        const bool ok = intersects( a, b );
        if ( ok )
            return true;
    }
    return false;
}







}















