
#include "pbd2_collision_utils.h"

namespace Pbd
{

static const Float EPS = 0.0001;

bool common_perp( const Vector3d & a1, const Vector3d & a2, const Vector3d & b1, const Vector3d & b2,
                  Vector3d & at_a, Vector3d & at_b )
{
	// Line vectors.
	const Vector3d da = a2 - a1;
	const Vector3d db = b2 - b1;
	// Common perpendicular line vector.
	const Vector3d nn = da.CrossProduct( db );
	const Float abs_n = nn.Length();
	if ( abs_n < EPS )
		return false;
	const Vector3d n = nn / abs_n;
	// Plane "a" normal.
	const Vector3d na = da.CrossProduct( n );
	// Plane "b" normal.
	const Vector3d nb = db.CrossProduct( n );
	// Common perpendicular is the intersection of the two planes.
	// Plane "a": (r - a1)*(na) = 0;
	// Plane "b": (r - b1)*(nb) = 0;
	// Substitute line "a" (r = da*t + a1) into plane "b" to find point on line "a".
	// Substitute line "b" (r = db*t + b1) intp plane "a" to find point on line "b".
	// Common perpendicular between line segments exists only if both
	// points "t" parameters are within [0, 1].
	const Float den_a = da.DotProduct(nb);
	if ( std::abs(den_a) < EPS )
		return false;
	const Float num_a = nb.DotProduct(b1 - a1);
	const Float ta = num_a / den_a;
	if ( (ta < 0.0) || (ta > 1.0) )
		return false;

	const Float den_b = db.DotProduct(na);
	if ( std::abs(den_b) < EPS )
		return false;
	const Float num_b = na.DotProduct(a1 - b1);
	const Float tb = num_b / den_b;
	if ( (tb < 0.0) || (tb > 1.0) )
		return false;

	at_a = da*ta + a1;
	at_b = db*tb + b1;
	return true;
}

bool BoxFace::intersects( const BoxVertex & a, const BoxVertex & b, Float & t_at, Vector3d & v_at ) const
{
	const Vector3d b_a = b.v-a.v;
	const Float den = n.DotProduct( b_a );
	if ( std::abs(den) < EPS )
		return false;
	const Float num = n.DotProduct(center - a.v);
	const Float t = num / den;
	if ( (t < 0.0) || (t > 1.0) )
		return false;
	const Vector3d at = a.v + (b_a*t);
	const Vector3d d_at = at - center;
	const Float dist_x = d_at.DotProduct( x );
	if ( std::abs(dist_x) > sz_x )
		return false;
	const Float dist_y = d_at.DotProduct( y );
	if ( std::abs(dist_y) > sz_y )
		return false;
	t_at = t;
	v_at = at;

	return true;
}


/*bool BoxFace::intersects( const BoxVertex & a, const BoxVertex & b, Vector3d & at, Vector3d & depth ) const
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

    // Now check if either vertices is inside and if closest face is the same face.
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
}*/

const int Box::vert_inds[12][2] = { {0, 1}, {1, 2}, {2, 3}, {3, 0}, 
							        {4, 5}, {5, 6}, {6, 7}, {7, 4}, 
							        {0, 4}, {1, 5}, {2, 6}, {3, 7} };

void Box::init( const Vector3d & sz2 )
{
    // Init 6 faces.
    {
		// Left
        BoxFace & v = faces[0];
        v.n = Vector3d( -1.0, 0.0, 0.0 );
        v.center = Vector3d( -sz2.x_, 0.0, 0.0 );
        v.x = Vector3d( 0.0, 0.0, 1.0 );
        v.y = Vector3d( 0.0, 1.0, 0.0 );
        v.sz_x = sz2.z_;
        v.sz_y = sz2.y_;
		v.edge_inds[0] = 0;
		v.edge_inds[1] = 8;
		v.edge_inds[2] = 4;
		v.edge_inds[3] = 9;
    }
    {
		// Back
        BoxFace & v = faces[1];
        v.n = Vector3d( 0.0, 0.0, 1.0 );
        v.center = Vector3d( 0.0, 0.0, sz2.z_ );
        v.x = Vector3d( 1.0, 0.0, 0.0 );
        v.y = Vector3d( 0.0, 1.0, 0.0 );
        v.sz_x = sz2.x_;
        v.sz_y = sz2.y_;
		v.edge_inds[0] = 1;
		v.edge_inds[1] = 10;
		v.edge_inds[2] = 5;
		v.edge_inds[3] = 9;
	}
    {
		// Right
        BoxFace & v = faces[2];
        v.n = Vector3d( 1.0, 0.0, 0.0 );
        v.center = Vector3d( sz2.x_, 0.0, 0.0 );
        v.x = Vector3d( 0.0, 0.0, -1.0 );
        v.y = Vector3d( 0.0, 1.0, 0.0 );
        v.sz_x = sz2.z_;
        v.sz_y = sz2.y_;
		v.edge_inds[0] = 2;
		v.edge_inds[1] = 11;
		v.edge_inds[2] = 6;
		v.edge_inds[3] = 10;
    }
    {
		// Forward
        BoxFace & v = faces[3];
        v.n = Vector3d( 0.0, 0.0, -1.0 );
        v.center = Vector3d( 0.0, 0.0, -sz2.z_ );
        v.x = Vector3d( -1.0, 0.0, 0.0 );
        v.y = Vector3d( 0.0, 1.0, 0.0 );
        v.sz_x = sz2.x_;
        v.sz_y = sz2.y_;
		v.edge_inds[0] = 3;
		v.edge_inds[1] = 11;
		v.edge_inds[2] = 7;
		v.edge_inds[3] = 8;
    }
    {
		// Down
        BoxFace & v = faces[4];
        v.n = Vector3d( 0.0, -1.0, 0.0 );
        v.center = Vector3d( 0.0, -sz2.y_, 0.0 );
        v.x = Vector3d( 1.0, 0.0, 0.0 );
        v.y = Vector3d( 0.0, 0.0, 1.0 );
        v.sz_x = sz2.x_;
        v.sz_y = sz2.z_;
		v.edge_inds[0] = 0;
		v.edge_inds[1] = 1;
		v.edge_inds[2] = 2;
		v.edge_inds[3] = 3;
    }
    {
		// Up
        BoxFace & v = faces[5];
        v.n = Vector3d( 0.0, 1.0, 0.0 );
        v.center = Vector3d( 0.0, sz2.y_, 0.0 );
        v.x = Vector3d( 1.0, 0.0, 0.0 );
        v.y = Vector3d( 0.0, 0.0, -1.0 );
        v.sz_x = sz2.x_;
        v.sz_y = sz2.z_;
		v.edge_inds[0] = 4;
		v.edge_inds[1] = 5;
		v.edge_inds[2] = 6;
		v.edge_inds[3] = 7;
	}

    // Init 8 vertices.
    {
        const Vector3d vv[] = { Vector3d(-sz2.x_, -sz2.y_, -sz2.z_), 
                                Vector3d(-sz2.x_, -sz2.y_,  sz2.z_), 
                                Vector3d( sz2.x_, -sz2.y_,  sz2.z_ ), 
                                Vector3d( sz2.x_, -sz2.y_, -sz2.z_), 
                                Vector3d(-sz2.x_,  sz2.y_, -sz2.z_), 
                                Vector3d(-sz2.x_,  sz2.y_,  sz2.z_), 
                                Vector3d( sz2.x_,  sz2.y_,  sz2.z_ ), 
                                Vector3d( sz2.x_,  sz2.y_, -sz2.z_) }; 
        for ( int i=0; i<8; i++ )
        {
            BoxVertex & v = verts[i];
            v.v = vv[i];
			v.inside = false;
			v.face_id = -1;
			v.depth = -1.0;
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
    int min_id = -1;
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
            min_id = i;
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
    return inside_const( v.v );
}

bool Box::inside_const( const Vector3d & v ) const
{
	for ( int i=0; i<6; i++ )
	{
		const BoxFace & f = faces[i];
		const Float nd = f.n.DotProduct( v - f.center );
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

	for ( int i=0; i<8; i++ )
	{
		const BoxVertex & v = verts[i];
		const bool ok = b.inside_const( v );
		if ( ok )
			return true;
	}

    for ( int i=0; i<12; i++ )
    {
        const int ind_a = vert_inds[i][0];
        const int ind_b = vert_inds[i][1];
        const BoxVertex & va = b.verts[ind_a];
        const BoxVertex & vb = b.verts[ind_b];
        for ( int j=0; j<6; j++ )
        {
            const BoxFace & f = faces[j];
			Float t_at;
            Vector3d v_at;
            const bool intersects = f.intersects( va, vb, t_at, v_at );
            if ( intersects )
                return true;
        }
    }

	for ( int i=0; i<12; i++ )
	{
		const int ind_a = vert_inds[i][0];
		const int ind_b = vert_inds[i][1];
		const BoxVertex & va = verts[ind_a];
		const BoxVertex & vb = verts[ind_b];
		for ( int j=0; j<6; j++ )
		{
			const BoxFace & f = b.faces[j];
			Float t_at;
			Vector3d v_at;
			const bool intersects = f.intersects( va, vb, t_at, v_at );
			if ( intersects )
				return true;
		}
	}

    return false;
}





}













