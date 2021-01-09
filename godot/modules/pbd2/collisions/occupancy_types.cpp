
#include "occupancy_types.h"


namespace Pbd
{

static const Float EPS = 0.000001;

static bool common_perpendicular( const Vector3d a0, const Vector3d & a1, const Vector3d & b0, const Vector3d & b1, Vector3d & ra, Vector3d & rb )
{
    {
        const Vector3d a1 = ra1 - ra0;
        const Vector3d a2 = rb1 - rb0;
        const Vector3  n = a1.CrossProduct( a1.CrossProduct(a2) );
        const Float den = a2.CotProduct( n );
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
        const Vector3  n = a1.CrossProduct( a1.CrossProduct(a2) );
        const Float den = a2.CotProduct( n );
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
    const Flaot abs_den = std::abs( den );
    if ( abs_den < EPS )
        return false;

    const Float num = d + norm.DotProduct( a );
    const Float t = num / den;
    if ( (t < 0.0) || (t > 1.0) )
        return false;

    at = a + (b - a)*t;
    return true;
}






void Face::init()
{
    plane.init( verts[0], verts[1], verts[2] );
    planes[0].init( vers[0], verts[1], verts[0]+plane.norm );
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
    static const edges[][] = { {0, 1}, {1, 2}, {2, 0} };
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
        intersecting_edge_index = i;
        // Determine which point is undernear.
        const Vector3d ba = b - a;
        const Float dir = ba.DotProduct( f.plane.norm );
        const bool head_inside = ( dir < 0.0 );
        // "head" inside, "tail" outside.
        const Vector3d tail = (tip_inside) ? a : b;
        head_a = (tip_inside) ? b : a;
        // Need to crop head within side plane bounds.
        for ( int j=0; j<3; j++ )
        {
            const Plane & pl = f.planes[j];
            Vector3d cropped_head;
            const bool intersects = pl.intersects( tail, head, cropped_head );
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
        const bool ok = intersects( a, b, at );
        if ( !ok )
            continue;
        // Remember intersecting edge index.
        intersecting_edge_index = i;
        // Determine which point is undernear.
        const Vector3d ba = b - a;
        const Float dir = ba.DotProduct( plane.norm );
        const bool head_inside = ( dir < 0.0 );
        // "head" inside, "tail" outside.
        const Vector3d tail = (tip_inside) ? a : b;
        head_b = (tip_inside) ? b : a;
        // Need to crop head within side plane bounds.
        for ( int j=0; j<3; j++ )
        {
            const Plane & pl = planes[j];
            Vector3d cropped_head;
            const bool intersects = pl.intersects( tail, head, cropped_head );
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
        const int_ind_a_0 = edges[intersecting_a_edge_index][0];
        const int_ind_a_1 = edges[intersecting_a_edge_index][1];
        const Vector3d & ra0 = verts[ind_ind_a_0];
        const Vector3d & ra1 = verts[ind_ind_a_1];
        // Other points.
        const int_ind_b_0 = edges[intersecting_b_edge_index][0];
        const int_ind_b_1 = edges[intersecting_b_edge_index][1];
        const Vector3d & rb0 = f.verts[ind_ind_b_0];
        const Vector3d & rb1 = f.verts[ind_ind_b_1];
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

        return true;
    }
    else if ( intersects_b )
    {

        return true;
    }

    return false;
}








void Cube::init()
{
    q = Quaterniond( 1.0, 0.0, 0.0, 0.0 );

    ex = Vector3d( 1.0, 0.0, 0.0 );
    ey = Vector3d( 0.0, 1.0, 0.0 );
    ez = Vector3d( 0.0, 0.0, 1.0 );
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


const Cube & Cube::operator*( const SE3 & se3 ) const
{
    Cube c = *this;
    c.origin = se3.r_ + origin;
    c.center = c.origin + se3.q_ * (center - origin);
    c.ex     = se3.q_ * ex;
    c.ey     = se3.q_ * ey;
    c.ez     = se3.q_ * ez;
    c.init_verts_and_planes();
    return c;
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
        const Float t = num / den;
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
        cosnt Plane & plane = planes[i];
        int out_qty = 0;
        for ( int j=0; j<8; j++ )
        {
            const Vector3d & pt = c.verts[j];
            const bool point_is_above = plane.above( pt );
            if ( point_is_above )
                out_qty += 1;
        }
        if ( out_qty > 7 )
            return false;
    }
    // Now check the other way.
    for ( int i=0; i<6; i++ )
    {
        cosnt Plane & plane = c.planes[i];
        int out_qty = 0;
        for ( int j=0; j<8; j++ )
        {
            const Vector3d & pt = verts[j];
            const bool point_is_above = plane.above( pt );
            if ( point_is_above )
                out_qty += 1;
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
            const above = plane.above( pt );
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
            const above = plane.above( pt );
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
    static const int edges[][] = { {0, 1}, {1, 2}, {2, 3}, {3, 0}, 
                                   {0, 4}, {1, 5}, {2, 6}, {3, 7}, 
                                   {4, 5}, {5, 6}, {6, 7}, {7, 4} };
    for ( int i=0; i<12; i++ )
    {
        const int ind_a = edges[i][0];
        const int ind_b = edges[i][1];
        const bool intersects = intersects( c.verts[ind_a], c.verts[ind_b] );
        if ( intersects )
            return true;
    }
    // And the other way around.
    for ( int i=0; i<12; i++ )
    {
        const int ind_a = edges[i][0];
        const int ind_b = edges[i][1];
        const bool intersects = c.intersects( verts[ind_a], verts[ind_b] );
        if ( intersects )
            return true;
    }

    return false;
}





}















