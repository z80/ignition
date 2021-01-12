
#include "pbd2_narrow_tree_node.h"
#include "pbd2_narrow_tree.h"
#include "matrix3d.h"

#include "SymmetricEigensolver3x3.h"

namespace Pbd
{

	static const Float EPS = 0.0001;

NarrowTreeNode::NarrowTreeNode()
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
}

NarrowTreeNode::~NarrowTreeNode()
{

}

NarrowTreeNode::NarrowTreeNode( const NarrowTreeNode & inst )
{
    *this = inst;
}

const NarrowTreeNode & NarrowTreeNode::operator=( const NarrowTreeNode & inst )
{
    if ( this != &inst )
    {
        tree = inst.tree;
        parentAbsIndex = inst.parentAbsIndex;
        indexInParent = inst.indexInParent;
        level = inst.level;
        value = inst.value;
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

        ptInds = inst.ptInds;

        init();
    }

    return *this;
}


void NarrowTreeNode::apply( const SE3 & se3 )
{
    cube_.apply( se3 );
    if ( value > 0 )
    {
        cube_optimized_.apply( se3 * se3_optimized_ );

        const int qty = ptInds.size();
        for ( int i=0; i<qty; i++ )
        {
            const int ind = ptInds.ptr()[i];
            Face & face = tree->faces_.ptrw()[ind];
            face.apply( se3 );
        }
    }
}



bool NarrowTreeNode::hasChildren() const
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

bool NarrowTreeNode::subdivide()
{
    // Subdrive means creating children.
    // It means node itself turns into not filled one.

    if ( level >= tree->max_depth_ )
        return false;
    
    // It can't have children because subdivision is happening.
    // But just in case.
    bool ch = hasChildren();
    if ( ch )
        return false;
    
    // If it is empty, no need to subdivide.
    if ( ptInds.empty() )
        return false;

	// Here initialize optimized cube "cube_optimized_" and its pose adjustment "se3_optimized_".
	compute_cube_optimized();
    
    // Don't subdivide if very few triangles.
    if ( ptInds.size() <= tree->min_triangles_ )
        return false;

    const int childLevel = this->level + 1;
    const Float chSize2  = this->size2 * 0.5;

    NarrowTreeNode nn[8];
    int    qtys[8];
    NarrowTreeNode n;
    for ( int i=0; i<8; i++ )
    {
        n.absIndex = tree->insert_node( n );
        nn[i] = tree->nodes_[ n.absIndex ];

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
    nn[2].center.x_ -= chSize2;
    nn[2].center.y_ += chSize2;
    nn[2].center.z_ -= chSize2;

    nn[3].center = this->center;
    nn[3].center.x_ += chSize2;
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
    nn[6].center.x_ -= chSize2;
    nn[6].center.y_ += chSize2;
    nn[6].center.z_ += chSize2;

    nn[7].center = this->center;
    nn[7].center.x_ += chSize2;
    nn[7].center.y_ += chSize2;
    nn[7].center.z_ += chSize2;

    for ( int i=0; i<8; i++ )
        nn[i].init();

    // Need to assign center and size
    const int qty = ptInds.size();
    for ( int j=0; j<8; j++ )
    {
        NarrowTreeNode & ch_n = nn[j];
        ch_n.value = 0;
        for ( int i=0; i<qty; i++ )
        {
            const int ind = ptInds[i];
            const Face & face = tree->faces_[ind];

            const bool inside = ch_n.inside( face );
            if ( inside )
            {
                ch_n.ptInds.push_back( ind );
                qtys[j] += 1;
                ch_n.value += 1;
            }
        }
        // This is probably debug code as it duplicates stuff above.
        if ( qtys[j] == 0 )
        {
           for ( int i=0; i<qty; i++ )
           {
               const int ind = ptInds[i];
               const Face & face = tree->faces_[ind];

               const bool inside = ch_n.inside( face );
               if ( inside )
               {
                   ch_n.ptInds.push_back( ind );
                   qtys[j] += 1;
                   ch_n.value += 1;
               }
           }
        }
    }

    // Reset indices and value for a node with children.
    ptInds.clear();
    value = 0;

    // Subdivide recursively.
    for ( int i=0; i<8; i++ )
    {
        NarrowTreeNode & ch_n = nn[i];
        if ( ( qtys[i] > 0 ) && ( childLevel < tree->max_depth_ ) )
            ch_n.subdivide();
        tree->nodes_.ptrw()[ ch_n.absIndex ] = ch_n;
    }

    return true;
}


bool NarrowTreeNode::inside( const Face & face ) const
{
    const bool intersects = cube_.intersects( face );
    return intersects;
}

bool NarrowTreeNode::inside( const NarrowTreeNode & n ) const
{
    const bool intersects = cube_.intersects( n.cube_ );
    if ( !intersects )
        return false;
    const bool has_ch = n.hasChildren();
    if ( !has_ch )
    {
        const bool is_filled = (n.value > 0);
        return is_filled;
    }

    for ( int i=0; i<8; i++ )
    {
        const int ind = n.children[i];
        const NarrowTreeNode & ch_n = tree->nodes_.ptr()[ind];
        const bool ch_intersects = inside( ch_n );
        if ( ch_intersects )
            return true;
    }

    return false;
}

void NarrowTreeNode::init()
{
    cube_.init( center, size2, size2, size2 );
}




bool NarrowTreeNode::collide_forward( const NarrowTreeNode & n, Vector<Vector3d> & pts, Vector<Vector3d> & depths ) const
{
    const bool intersects = cube_.intersects( n.cube_ );
    if ( !intersects )
        return false;
    const bool has_ch = n.hasChildren();
    if ( !has_ch )
    {
        const bool is_filled = (n.value > 0);
        if ( !is_filled )
            return false;
        const bool ret = n.collide_backward( *this, pts, depths );
        return ret;
    }

    bool children_intersect = false;
    for ( int i=0; i<8; i++ )
    {
        const int ind = n.children[i];
        const NarrowTreeNode & child_node = tree->nodes_.ptr()[ind];
        const bool ch_intersects = collide_forward( child_node, pts, depths );
        children_intersect = children_intersect || ch_intersects;
    }

    return children_intersect;
}

bool NarrowTreeNode::collide_backward( const NarrowTreeNode & this_node, Vector<Vector3d> & pts, Vector<Vector3d> & depths ) const
{
    const bool intersects = cube_.intersects( this_node.cube_ );
    if ( !intersects )
        return false;
    const bool has_ch = this_node.hasChildren();
    if ( !has_ch )
    {
        const bool is_filled = (this_node.value > 0);
        if ( !is_filled )
            return false;
        // Colliding individual faces.
        const bool ret = collide_faces( this_node, pts, depths );
        return ret;
    }

    bool children_intersect = false;
    for ( int i=0; i<8; i++ )
    {
        const int ind = this_node.children[i];
        const NarrowTreeNode & child_node = tree->nodes_.ptr()[ind];
        const bool ch_intersects = collide_backward( child_node, pts, depths );
        children_intersect = children_intersect || ch_intersects;
    }

    return children_intersect;

    return true;
}


bool NarrowTreeNode::collide_faces( const NarrowTreeNode & this_node, Vector<Vector3d> & pts, Vector<Vector3d> & depths ) const
{
    const NarrowTreeNode & other_node = *this;
    const int own_qty   = this_node.ptInds.size();
    const int other_qty = other_node.ptInds.size();
    Vector3d face_pts[3];
    Vector3d face_depths[3];
    
    bool ret = false;
    for ( int i=0; i<own_qty; i++ )
    {
        const int this_ind = this_node.ptInds.ptr()[i];
        const Face & this_face = tree->faces_.ptr()[this_ind];
        for ( int j=0; j<other_qty; j++ )
        {
            const int other_ind = other_node.ptInds.ptr()[j];
            const Face & other_face = tree->faces_.ptr()[other_ind];
            const int qty = this_face.intersects_all( other_face, face_pts, face_depths );
            ret = ret || (qty > 0);
            for ( int k=0; k<qty; k++ )
            {
                const Vector3d & pt    = face_pts[k];
                const Vector3d & depth = face_depths[k];
                pts.push_back( pt );
                depths.push_back( depth );
            }
        }
    }
    return ret;
}


static bool compute_mean_and_std( NarrowTreeNode & node, Vector3d & std_diag, Vector3d & std_off_diag );

bool NarrowTreeNode::compute_cube_optimized()
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





static bool compute_mean_and_std( NarrowTreeNode & node, Vector3d & std_diag, Vector3d & std_off_diag )
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



