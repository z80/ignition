
#include "pbd2_narrow_tree_pts_node.h"
#include "pbd2_narrow_tree.h"
#include "matrix3d.h"

#include "SymmetricEigensolver3x3.h"

namespace Pbd
{

	static const Float EPS = 0.0001;

NarrowTreePtsNode::NarrowTreePtsNode()
{
    this->tree = nullptr;
    parentAbsIndex = -1;
    indexInParent = -1;
    level = -1;
    absIndex = -1;

    for ( int i=0; i<8; i++ )
        children[i] = -1;

    size2 = 1.0;
    center = Vector3( 0.0, 0.0, 0.0 );

    init();
	reset_distances();
}

NarrowTreePtsNode::~NarrowTreePtsNode()
{

}

NarrowTreePtsNode::NarrowTreePtsNode( const NarrowTreePtsNode & inst )
{
    *this = inst;
}

const NarrowTreePtsNode & NarrowTreePtsNode::operator=( const NarrowTreePtsNode & inst )
{
    if ( this != &inst )
    {
        tree = inst.tree;
        parentAbsIndex = inst.parentAbsIndex;
        indexInParent = inst.indexInParent;
        level = inst.level;
        absIndex = inst.absIndex;

        children[0] = inst.children[0];
        children[1] = inst.children[1];
        children[2] = inst.children[2];
        children[3] = inst.children[3];
        children[4] = inst.children[4];
        children[5] = inst.children[5];
        children[6] = inst.children[6];
        children[7] = inst.children[7];

        size2 = inst.size2;
        center = inst.center;
        
        cube_ = inst.cube_;
        se3_optimized_  = inst.se3_optimized_;
        cube_optimized_ = inst.cube_optimized_;

		d000 = inst.d000;
		d100 = inst.d100;
		d110 = inst.d110;
		d010 = inst.d010;
		d001 = inst.d001;
		d101 = inst.d101;
		d111 = inst.d111;
		d011 = inst.d011;

        ptInds = inst.ptInds;

        init();
    }

    return *this;
}




bool NarrowTreePtsNode::hasChildren() const
{
    // Filled can't have children.
    if ( !ptInds.empty() )
        return false;

    for ( int i=0; i<8; i++ )
    {
        if ( children[i] >= 0 )
            return true;
    }

    return false;
}

bool NarrowTreePtsNode::subdivide()
{
    if ( level >= tree->max_depth_ )
	{
		if ( !ptInds.empty() )
			compute_cube_optimized();
        return false;
	}
	if ( ptInds.size() < tree->min_pts_ )
	{
		if ( !ptInds.empty() )
			compute_cube_optimized();
		return false;
	}
    
    // It can't have children because subdivision is happening.
    // But just in case.
    bool ch = hasChildren();
    if ( ch )
        return false;
    
    // If it is empty, no need to subdivide.
    if ( ptInds.empty() )
        return false;

    const int childLevel = this->level + 1;
    const Float chSize2  = this->size2 * 0.5;

    NarrowTreePtsNode nn[8];
    int    qtys[8];
    NarrowTreePtsNode n;
    for ( int i=0; i<8; i++ )
    {
        n.absIndex = tree->insert_node_pts( n );
        nn[i] = tree->nodes_pts_[ n.absIndex ];

        nn[i].indexInParent = i;
        nn[i].parentAbsIndex = this->absIndex;
        nn[i].level = childLevel;
        nn[i].size2 = chSize2;

        children[i] = nn[i].absIndex;

        qtys[i] = 0;
    }
    nn[0].center = this->center;
    nn[0].center.x_ -= chSize2;
    nn[0].center.y_ -= chSize2;
    nn[0].center.z_ -= chSize2;

    nn[1].center = this->center;
    nn[1].center.x_ += chSize2;
    nn[1].center.y_ -= chSize2;
    nn[1].center.z_ -= chSize2;

    nn[2].center = this->center;
    nn[2].center.x_ += chSize2;
    nn[2].center.y_ += chSize2;
    nn[2].center.z_ -= chSize2;

    nn[3].center = this->center;
    nn[3].center.x_ -= chSize2;
    nn[3].center.y_ += chSize2;
    nn[3].center.z_ -= chSize2;

    nn[4].center = this->center;
    nn[4].center.x_ -= chSize2;
    nn[4].center.y_ -= chSize2;
    nn[4].center.z_ += chSize2;

    nn[5].center = this->center;
    nn[5].center.x_ += chSize2;
    nn[5].center.y_ -= chSize2;
    nn[5].center.z_ += chSize2;

    nn[6].center = this->center;
    nn[6].center.x_ += chSize2;
    nn[6].center.y_ += chSize2;
    nn[6].center.z_ += chSize2;

    nn[7].center = this->center;
    nn[7].center.x_ -= chSize2;
    nn[7].center.y_ += chSize2;
    nn[7].center.z_ += chSize2;

    for ( int i=0; i<8; i++ )
        nn[i].init();

    // Need to assign center and size
    const int qty = ptInds.size();
    for ( int j=0; j<8; j++ )
    {
        NarrowTreePtsNode & ch_n = nn[j];
        for ( int i=0; i<qty; i++ )
        {
            const int ind = ptInds[i];
			const Vector3d & pt = tree->pts_[ind];

			const bool inside = ch_n.point_inside( pt );
            if ( inside )
            {
                ch_n.ptInds.push_back( ind );
                qtys[j] += 1;
            }
        }
        // This is probably debug code as it duplicates stuff above.
        if ( qtys[j] == 0 )
        {
           for ( int i=0; i<qty; i++ )
           {
               const int ind = ptInds[i];
               const Vector3d & pt = tree->pts_[ind];

               const bool inside = ch_n.point_inside( pt );
               if ( inside )
               {
                   ch_n.ptInds.push_back( ind );
                   qtys[j] += 1;
               }
           }
        }
    }

    // Reset indices and value for a node with children.
    ptInds.clear();

    // Subdivide recursively.
    for ( int i=0; i<8; i++ )
    {
        NarrowTreePtsNode & ch_n = nn[i];
        if ( ( qtys[i] > 0 ) && ( childLevel < tree->max_depth_ ) )
            ch_n.subdivide();
        tree->update_node_pts( ch_n );
    }

    return true;
}


bool NarrowTreePtsNode::point_inside( const Vector3d & at ) const
{
	const bool ret = cube_.contains( at );
	return ret;
}

void NarrowTreePtsNode::init()
{
    cube_.init( center, size2, size2, size2 );
}










bool NarrowTreePtsNode::collide_backward( const SE3 & se3_rel, const NarrowTreeSdfNode * this_node, Vector<Vector3d> & pts, Vector<Vector3d> & depths ) const
{
	// Apply transforms.
	Cube this_cube = this_node->cube_;
	this_cube.apply( se3_rel );

    const bool intersects = cube_.intersects( this_cube );
    if ( !intersects )
        return false;

    const bool has_ch = this_node->hasChildren();
    if ( !has_ch )
    {
        const bool is_on_or_bleow_surf = this_node->on_or_below_surface;
        if ( !is_on_or_bleow_surf )
            return false;

		// Last thing is check optimized cube.
		const bool intersects_optimized = cube_optimized_.intersects( this_cube );
		if ( !intersects_optimized )
			return false;
        // Colliding individual faces.
        const bool ret = collide_points( this_node, pts, depths );
        return ret;
    }

    bool children_intersect = false;
    for ( int i=0; i<8; i++ )
    {
        const int ind = this_node->children[i];
        const NarrowTreeSdfNode * child_node = &( this_node->tree->nodes_sdf_.ptrw()[ind] );
        const bool ch_intersects = collide_backward( child_node, pts, depths );
        children_intersect = children_intersect || ch_intersects;
    }

    return children_intersect;
}


bool NarrowTreePtsNode::collide_points( const NarrowTreeSdfNode * this_node, Vector<Vector3d> & pts, Vector<Vector3d> & depths ) const
{
	bool ret = false;
	const SE3 se3_rel = tree->se3_ / this_node->tree->se3_;
	const int qty = ptInds.size();
	for ( int i=0; i<qty; i++ )
	{
		const int ind = ptInds.ptr()[i];
		const Vector3d & pt = tree->min_pts_.ptr()[ind];
		const Vector3d pt_local = (se3.q_ * pt) + se3.r_;

		Vector3d depth;
		const Float d = this_node->distance_for_this_node( pt_local, depth );
		if ( d < 0.0 )
		{
			// Convert to global ref. frame.
			const SE3 & se3 = this_node->tree->se3_;
			const Vector3d pt_global = ( se3.q_ * pt_local ) + se3.r_;
			pts.push_back( pt_global );
			// Convert to global ref. frame.
			depth = se3.q_ * depth;
			depths.push_back( depth );

			ret = true;
		}
	}

	return ret;
}


static bool compute_mean_and_std( NarrowTreePtsNode & node, Vector3d & std_diag, Vector3d & std_off_diag );

bool NarrowTreePtsNode::compute_cube_optimized()
{
	Vector3d std_diag;
	Vector3d std_off_diag;
	const bool std_ok = compute_mean_and_std( *this, std_diag, std_off_diag );
	if ( !std_ok )
		return false;

	std::array<Float, 3> eval                = { 0.0, 0.0, 0.0 };
	std::array<std::array<Float, 3>, 3> evec; //= { {0.0, 0.0, 0.0},
	                                          //   {0.0, 0.0, 0.0},
	                                          //   {0.0, 0.0, 0.0} };
	gte::SymmetricEigensolver3x3<Float> solver;
	const int ret = solver( std_diag.x_, std_off_diag.x_, std_off_diag.y_, std_diag.y_, std_off_diag.z_, std_diag.z_,
		                    false, -1, eval, evec );
	if ( ret < 1 )
		return false;

	Matrix3d A;
	A.m00_ = std_diag.x_;
	A.m11_ = std_diag.y_;
	A.m22_ = std_diag.z_;

	A.m01_ = std_off_diag.x_;
	A.m02_ = std_off_diag.y_;
	A.m12_ = std_off_diag.z_;

	A.m10_ = std_off_diag.x_;
	A.m20_ = std_off_diag.y_;
	A.m21_ = std_off_diag.z_;

	Quaterniond Q;
	Q.FromRotationMatrix( A );
	Q.Normalize();

	const Vector3d ex = Q * Vector3d( 1.0, 0.0, 0.0 );
	const Vector3d ey = Q * Vector3d( 0.0, 1.0, 0.0 );
	const Vector3d ez = Q * Vector3d( 0.0, 0.0, 1.0 );
	Float x_min, x_max, y_min, y_max, z_min, z_max;

	// Initialize with the very first point.
	{
		const int ind0 = ptInds.ptr()[0];
		const Face & f0 = tree->faces_.ptr()[ind0];
		const Vector3d v0 = f0.verts_0[0];
		x_min = x_max = ex.DotProduct( v0 );
		y_min = y_max = ey.DotProduct( v0 );
		z_min = z_max = ez.DotProduct( v0 );
	}

	const int qty = ptInds.size();
	for ( int i=0; i<qty; i++ )
	{
		const int ind = ptInds.ptr()[i];
		const Face & f = tree->faces_.ptr()[ind];
		for ( int j=0; j<3; j++ )
		{
			const Vector3d v = f.verts_0[j];
			const Float x = v.DotProduct( ex );
			const Float y = v.DotProduct( ey );
			const Float z = v.DotProduct( ez );
			if ( x < x_min )
				x_min = x;
			if ( x > x_max )
				x_max = x;
			if ( y < y_min )
				y_min = y;
			if ( y > y_max )
				y_max = y;
			if ( z < z_min )
				z_min = z;
			if ( z > z_max )
				z_max = z;
		}
	}

	Vector3d r_adj( (x_min+x_max)/2.0, (y_min+y_max)/2.0, (z_min+z_max)/2.0 );
	r_adj -= cube_.center_0;
	Float szx2 = (x_max - x_min)/2.0;
	if ( szx2 < EPS )
		szx2 = EPS;
	Float szy2 = (y_max - y_min)/2.0;
	if ( szy2 < EPS )
		szy2 = EPS;
	Float szz2 = (z_max - z_min)/2.0;
	if ( szz2 < EPS )
		szz2 = EPS;

	cube_optimized_.init( cube_.center_0, szx2, szy2, szz2 );
	se3_optimized_.r_ = r_adj;
	se3_optimized_.q_ = Q;

	return false;
}

void NarrowTreePtsNode::reset_distances()
{
	d000 = d100 = d110 = d010 = 
	d001 = d101 = d111 = d011 = -1.0;
}

void NarrowTreePtsNode::init_distances()
{
	// Initialize distances for the root node
	// and for leaf nodes.
	const bool has_children = hasChildren();

	if ( (absIndex == 0) || (!has_children) )
	{
		const int faces_qty = tree->faces_.size();
		for ( int i=0; i<8; i++ )
		{
			const Vector3d & v = cube_.verts[i];
			// Check if the point is inside the mesh.
			const bool inside_mesh = point_inside_mesh( v );
			if ( inside_mesh )
				int aaa = 0;
			// Compute the smallest distance to the surface.
			Float min_dist = -1.0;
			for ( int j=0; j<faces_qty; j++ )
			{
				const Face & f = tree->faces_.ptr()[j];
				const Float d = f.distance( v );
				if ( (min_dist < 0.0) || (min_dist > d) )
					min_dist = d;
			}
			// If point is inside the mesh, point is by definition negative.
			if ( inside_mesh )
				min_dist = -min_dist;

			switch ( i )
			{
			case 0:
				d000 = min_dist;
				break;
			case 1:
				d100 = min_dist;
				break;
			case 2:
				d110 = min_dist;
				break;
			case 3:
				d010 = min_dist;
				break;
			case 4:
				d001 = min_dist;
				break;
			case 5:
				d101 = min_dist;
				break;
			case 6:
				d111 = min_dist;
				break;
			default:
				d011 = min_dist;
				break;
			}
		}
	}
	if ( !has_children )
		return;

	for ( int i=0; i<8; i++ )
	{
		const int ch_ind = children[i];
		NarrowTreePtsNode & n = tree->nodes_sdf_.ptrw()[ch_ind];
		n.init_distances();
	}
}

bool NarrowTreePtsNode::distance( const Vector3d & r, Float & d, Vector3d & dist ) const
{
	Vector3d rel_r;
	const bool ok = distance_recursive( r, d, rel_r );
	if ( !ok )
		return false;
	dist = tree->se3_.q_ * rel_r;
	return true;
}

bool NarrowTreePtsNode::distance_recursive( const Vector3d & r, Float & d, Vector3d & ret ) const
{
	const bool inside = cube_.contains( r );
	if ( !inside )
		return false;

	// If point is inside check if the node is leaf node.
	const bool has_children = hasChildren();
	if ( !has_children )
	{
		// Compute distance for this node.
		d = distance_for_this_node( r, ret );
		return true;
	}

	// Min distance means the deepest inside the body.
	bool min_dist_initialized = false;
	Float min_dist = 0.0;
	Vector3d min_ret;
	for ( int i=0; i<8; i++ )
	{
		const int ch_ind = children[i];
		const NarrowTreePtsNode & child_node = tree->nodes_sdf_.ptr()[ch_ind];
		const bool ok = child_node.distance_recursive( r, min_dist, min_ret );
		if ( ok )
		{
			min_dist_initialized = true;
			break;
		}
	}

	if ( !min_dist_initialized )
		return false;

	d = min_dist;
	ret = min_ret;
	return true;
}

Float NarrowTreePtsNode::distance_for_this_node( const Vector3d & r, Vector3d & disp ) const
{
	// Input must be normalized to [-1, 1] with zero at the center.
	const Vector3d rel_r = r - cube_.center;
	const Float x = rel_r.DotProduct( cube_.ex ) / cube_.szx2;
	const Float y = rel_r.DotProduct( cube_.ey ) / cube_.szy2;
	const Float z = rel_r.DotProduct( cube_.ez ) / cube_.szz2;

	const Float d = ((((d111 - d110 - d101 + d100 - d011 + d010 + d001 - d000)*x + d111
		- d110 - d101 + d100 + d011 - d010 + d001 + d000)*y
		+ (d111 - d110 + d101 - d100 - d011 + d010 - d001 + d000)*x + d111 - d110
		+ d101 - d100 + d011 - d010 - d001 - d000)*z
		+ ((d111 + d110 - d101 - d100 - d011 - d010 + d001 + d000)*x + d111 + d110
			- d101 - d100 + d011 + d010 + d001 - d000)*y
		+ (d111 + d110 + d101 + d100 - d011 - d010 - d001 - d000)*x + d111 + d110
		+ d101 + d100 + d011 + d010 - d001 + d000)/8.0;

	const Float dx = (((d111 - d110 - d101 + d100 - d011 + d010 + d001 - d000)*y + d111 - d110
		+ d101 - d100 - d011 + d010 - d001 + d000)*z
		+ (d111 + d110 - d101 - d100 - d011 - d010 + d001 + d000)*y + d111 + d110
		+ d101 + d100 - d011 - d010 - d001 - d000)/8.0;

	const Float dy = (((d111 - d110 - d101 + d100 - d011 + d010 + d001 - d000)*x + d111
		- d110 - d101 + d100 + d011 - d010 + d001 + d000)*z
		+ (d111 + d110 - d101 - d100 - d011 - d010 + d001 + d000)*x + d111 + d110
		- d101 - d100 + d011 + d010 + d001 - d000)/8.0;

	const Float dz = (((d111 - d110 - d101 + d100 - d011 + d010 + d001 - d000)*x + d111
		- d110 - d101 + d100 + d011 - d010 + d001 + d000)*y
		+ (d111 - d110 + d101 - d100 - d011 + d010 - d001 + d000)*x + d111 - d110
		+ d101 - d100 + d011 - d010 - d001 - d000)/8.0;

	const Vector3d local_dr( dx, dy, dz );
	const Float L = local_dr.Length();
	Vector3d local_disp;
	if ( L > 0.000001 )
	{
		// Towards the surface. And absolute value is equal to
		// the displacement.
		local_disp = local_dr * ( -d/L );
	}
	// Apply basis.
	disp = cube_.ex*local_disp.x_ + cube_.ey*local_disp.y_ + cube_.ez*local_disp.z_;
	return d;
}


bool NarrowTreePtsNode::point_inside_mesh( const Vector3d & r ) const
{
	const NarrowTreePtsNode & n0 = tree->nodes_sdf_.ptr()[0];
	const Float max_d = 4.0 * n0.size2;
	const Vector3d r2 = r + Vector3d( max_d, 0.0, 0.0 );

	const int faces_qty = tree->faces_.size();
	int intersections_qty = 0;
	for ( int i=0; i<faces_qty; i++ )
	{
		const Face & tri = tree->faces_.ptr()[i];
		Vector3d at;
		const bool intersects = tri.intersects( r, r2, at );
		if ( intersects )
			intersections_qty += 1;
	}
	const bool inside = ( (intersections_qty & 1) != 0 );
	return inside;
}

bool NarrowTreePtsNode::point_inside( const Vector3d & at ) const
{
	const bool ret = cube_.contains( at );
	return ret;
}

bool NarrowTreePtsNode::intersects_ray( const Vector3d & r1, const Vector3d & r2 ) const
{
	if ( !ptInds.empty() )
	{
		// Check bounding boxes.
		const bool ret = cube_.intersects( r1, r2 );
		return ret;
	}
	if ( !hasChildren() )
		return false;
	for ( int i=0; i<8; i++ )
	{
		const int ch_ind = children[i];
		const NarrowTreePtsNode & child_node = tree->nodes_sdf_.ptr()[ch_ind];
		const bool ret = child_node.intersects_ray( r1, r2 );
		if ( ret )
			return true;
	}
	return false;
}






static bool compute_mean_and_std( NarrowTreePtsNode & node, Vector3d & std_diag, Vector3d & std_off_diag )
{
	const int qty = node.ptInds.size();
	if ( qty < 0 )
		return false;

	Vector3d m = Vector3d( 0.0, 0.0, 0.0 );
	for ( int i=0; i<qty; i++ )
	{
		const int ind = node.ptInds.ptr()[i];
		const Face & f = node.tree->faces_.ptr()[ind];
		for ( int j=0; j<3; j++ )
		{
			const Vector3d & v = f.verts_0[j];
			m += v;
		}
	}
	const Float inv_qty = 1.0 / static_cast<Float>(qty);
	m *= inv_qty;

	Matrix3d std;
	std.m00_ = 0.0;
	std.m01_ = 0.0;
	std.m02_ = 0.0;
	std.m10_ = 0.0;
	std.m11_ = 0.0;
	std.m12_ = 0.0;
	std.m20_ = 0.0;
	std.m21_ = 0.0;
	std.m22_ = 0.0;
	for ( int i=0; i<qty; i++ )
	{
		const int ind = node.ptInds.ptr()[i];
		const Face & f = node.tree->faces_.ptr()[ind];
		for ( int j=0; j<3; j++ )
		{
			const Vector3d v = f.verts_0[j] - m;
			std.m00_ += v.x_*v.x_;
			std.m11_ += v.y_*v.y_;
			std.m22_ += v.z_*v.z_;
			std.m01_ += v.x_*v.y_;
			std.m02_ += v.x_*v.z_;
			std.m12_ += v.y_*v.z_;
		}
	}
	std.m00_ *= inv_qty;
	std.m11_ *= inv_qty;
	std.m22_ *= inv_qty;
	std.m01_ *= inv_qty;
	std.m02_ *= inv_qty;
	std.m12_ *= inv_qty;

	std.m00_ = std::sqrt( std.m00_ );
	std.m11_ = std::sqrt( std.m11_ );
	std.m22_ = std::sqrt( std.m22_ );
	std.m01_ = std::sqrt( std.m01_ );
	std.m02_ = std::sqrt( std.m02_ );
	std.m12_ = std::sqrt( std.m12_ );

	std_diag.x_ = std.m00_;
	std_diag.y_ = std.m11_;
	std_diag.z_ = std.m22_;
	std_off_diag.x_ = std.m01_;
	std_off_diag.y_ = std.m02_;
	std_off_diag.z_ = std.m12_;

	return true;
}





}



