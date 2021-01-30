
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

void Box::collisions_with( const Box & box_b, Vector<Vector3d> & ats, Vector<Vector3d> & depths )
{
	verts_a_inside_b.clear();
	edges_a_crossing_b.clear();
	verts_b_inside_a.clear();
	edges_b_crossing_a.clear();

	const int pts_qty = all_box_intersections( *this, box_b,
		verts_a_inside_b, edges_a_crossing_b,
		verts_b_inside_a, edges_b_crossing_a );

	// Process intersections with box "a".
	{
		const int face_ind = box_nearest_face( *this, edges_a_crossing_b, edges_b_crossing_a );
		const BoxFace & f = this->faces[face_ind];
		// Vertices "b" inside "a".
		const int verts_qty = verts_b_inside_a.size();
		for ( int i=0; i<verts_qty; i++ )
		{
			const int ind = verts_b_inside_a.ptr()[i];
			const BoxVertex & v = box_b.verts[ind];
			// Project onto the face normal.
			const Float d = f.n.DotProduct( v.v - f.center );
			// It should be towards "a".
			const Vector3d depth = (d * f.n);
			const Vector3d at = v.v + (depth * 0.5);
			ats.push_back( at );
			depths.push_back( depth );
		}
		// Edges "b" crossing "a".
		const int edges_qty = edges_b_crossing_a.size();
		for ( int i=0; i<edges_qty; i++ )
		{
			const Vector3d & pt = edges_b_crossing_a.ptr()[i];
			// Distance to the face.
			const Float d = f.n.DotProduct( pt - f.center );
			// It should be towards "a".
			const Vector3d depth = (d * f.n);
			const Vector3d at = pt + (depth * 0.5);
			ats.push_back( at );
			depths.push_back( depth );
		}
	}
	// And now with "b".
	{
		const int face_ind = box_nearest_face( box_b, edges_a_crossing_b, edges_b_crossing_a );
		const BoxFace & f = box_b.faces[face_ind];
		// Vertices "a" inside "b".
		const int verts_qty = verts_a_inside_b.size();
		for ( int i=0; i<verts_qty; i++ )
		{
			const int ind = verts_a_inside_b.ptr()[i];
			const BoxVertex & v = this->verts[ind];
			// Project onto the face normal.
			const Float d = f.n.DotProduct( v.v - f.center );
			// It should be towards "a".
			const Vector3d depth = -(d * f.n);
			const Vector3d at = v.v - (depth * 0.5);
			ats.push_back( at );
			depths.push_back( depth );
		}
		// Edges "b" crossing "a".
		const int edges_qty = edges_a_crossing_b.size();
		for ( int i=0; i<edges_qty; i++ )
		{
			const Vector3d & pt = edges_a_crossing_b.ptr()[i];
			// Distance to the face.
			const Float d = f.n.DotProduct( pt - f.center );
			// It should be towards "a".
			const Vector3d depth = -(d * f.n);
			const Vector3d at = pt - (depth * 0.5);
			ats.push_back( at );
			depths.push_back( depth );
		}
	}
}

int Box::all_box_intersections( const Box & box_a, const Box & box_b,
	Vector<int> & verts_a_inside_b, 
	Vector<Vector3d> & edges_a_crossing_b,
	Vector<int> & verts_b_inside_a, 
	Vector<Vector3d> & edges_b_crossing_a )
{
	// Verts "a" inside "b" and vise versa.
	bool verts_a_inside_b_bool[8];
	bool verts_b_inside_a_bool[8];
	for ( int i=0; i<8; i++ )
	{
		const BoxVertex & va = box_a.verts[i];
		const bool v_a_inside_b = box_b.inside_const( va );
		verts_a_inside_b_bool[i] = v_a_inside_b;
		if ( v_a_inside_b )
			verts_a_inside_b.push_back( i );

		const BoxVertex & vb = box_b.verts[i];
		const bool v_b_inside_a = box_a.inside_const( vb );
		verts_b_inside_a_bool[i] = v_b_inside_a;
		if ( v_b_inside_a )
			verts_b_inside_a.push_back( i );
	}
	// Edges of box "a" against faces of box "b" and vise versa.
	int intersections_found_qty = 0;
	for ( int i=0; i<12; i++ )
	{
		const int ind1 = Box::vert_inds[i][0];
		const int ind2 = Box::vert_inds[i][1];
		const BoxVertex & va1 = box_a.verts[ind1];
		const BoxVertex & va2 = box_a.verts[ind2];
		// Collide with all faces of box "b".
		// But keep in mind that there is not more than 2 collisions.
		// And if either vertex is inside, there is no more than 1 collision.
		const bool va1_inside = verts_a_inside_b_bool[ind1];
		const bool va2_inside = verts_a_inside_b_bool[ind2];
		if ( (!va1_inside) || (!va2_inside) )
		{
			const bool look_for_one = va1_inside ^ va2_inside;
			const int max_qty = look_for_one ? 1 : 2;
			int edge_intersections_qty = 0;
			for ( int j=0; j<6; j++ )
			{
				const BoxFace & f = box_b.faces[j];
				Float t_at;
				Vector3d v_at;
				const bool intersects = f.intersects( va1, va2, t_at, v_at );
				if ( intersects )
				{
					edges_a_crossing_b.push_back( v_at );
					edge_intersections_qty += 1;
					if ( edge_intersections_qty >= max_qty )
						break;
				}
			}
			intersections_found_qty += edge_intersections_qty;
		}

		// Now the same. Edges of "b" crossing faces of "a".
		const BoxVertex & vb1 = box_b.verts[ind1];
		const BoxVertex & vb2 = box_b.verts[ind2];
		const bool vb1_inside = verts_b_inside_a_bool[ind1];
		const bool vb2_inside = verts_b_inside_a_bool[ind2];
		if ( (!vb1_inside) || (!vb2_inside) )
		{
			const bool look_for_one = vb1_inside ^ vb2_inside;
			const int max_qty = look_for_one ? 1 : 2;
			int edge_intersections_qty = 0;
			for ( int j=0; j<6; j++ )
			{
				const BoxFace & f = box_a.faces[j];
				Float t_at;
				Vector3d v_at;
				const bool intersects = f.intersects( vb1, vb2, t_at, v_at );
				if ( intersects )
				{
					edges_b_crossing_a.push_back( v_at );
					edge_intersections_qty += 1;
					if ( edge_intersections_qty >= max_qty )
						break;
				}
			}
			intersections_found_qty += edge_intersections_qty;
		}
	}

	return intersections_found_qty;
}

int Box::box_nearest_face( const Box & box, 
						   const Vector<Vector3d> & edges_a_crossing_b,
						   const Vector<Vector3d> & edges_b_crossing_a )
{
	// Compute mean of all points.
	Vector3d mean = Vector3d::ZERO;
	const int qty_a = edges_a_crossing_b.size();
	for ( int i=0; i<qty_a; i++ )
	{
		const Vector3d & pt = edges_a_crossing_b.ptr()[i];
		mean += pt;
	}
	const int qty_b = edges_b_crossing_a.size();
	for ( int i=0; i<qty_b; i++ )
	{
		const Vector3d & pt = edges_b_crossing_a.ptr()[i];
		mean += pt;
	}
	mean /= static_cast<Float>(qty_a + qty_b);

	// Compute the size of the vertical from a face to this "mean". The one for which
	// the length of this vertical is the smallest is the one needed.
	Float min_d = -1.0;
	int min_face_index = -1;
	for ( int face_ind=0; face_ind<6; face_ind++ )
	{
		const BoxFace & f = box.faces[face_ind];
		Float accum_d = 0.0;
		for ( int i=0; i<qty_a; i++ )
		{
			const Vector3d & pt = edges_a_crossing_b.ptr()[i];
			const Float d = f.n.DotProduct( pt - f.center );
			accum_d += std::abs( d );
		}
		const int qty_b = edges_b_crossing_a.size();
		for ( int i=0; i<qty_b; i++ )
		{
			const Vector3d & pt = edges_b_crossing_a.ptr()[i];
			const Float d = f.n.DotProduct( pt - f.center );
			accum_d += std::abs( d );
		}
		if ( (min_face_index < 0) || (accum_d < min_d) )
		{
			min_face_index = face_ind;
			min_d = accum_d;
		}
	}

	return min_face_index;
}





}













