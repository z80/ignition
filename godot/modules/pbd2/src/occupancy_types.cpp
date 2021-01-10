
#include "occupancy_types.h"


namespace Pbd
{

static const Float EPS = 0.000001;

static bool common_perpendicular( const Vector3d & ra0, const Vector3d & ra1, const Vector3d & rb0, const Vector3d & rb1, Vector3d & ra, Vector3d & rb )
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

    const Float num = d + norm.DotProduct( a );
    const Float t = num / den;
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










void Cube::init( const Vector3d & c, Float x2, Float y2, Float z2 )
{
    center = c;
    szx2 = x2;
    szy2 = y2;
    szz2 = z2;
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
        const Plane & plane = planes[i];
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
        const Plane & plane = c.planes[i];
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















