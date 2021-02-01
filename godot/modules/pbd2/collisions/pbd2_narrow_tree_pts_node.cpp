
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
        cube_optimized_ = inst.cube_optimized_;

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










bool NarrowTreePtsNode::collide_backward( const Pose & se3_rel, const NarrowTreeSdfNode * this_node, Vector<Vector3d> & pts, Vector<Vector3d> & depths ) const
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
        const bool is_on_or_below_surf = this_node->on_or_below_surface;
        if ( !is_on_or_below_surf )
            return false;

        // Last thing is check optimized cube.
        // <<<<<<<<< FOR NOW WITHOUT OPTIMIZED BOXES >>>>>>>>>>>>>>
        //const bool intersects_optimized = cube_optimized_.intersects( this_cube );
        //if ( !intersects_optimized )
        //      return false;
        // Colliding individual faces.
        const bool ret = collide_points( this_node, pts, depths );
        return ret;
    }

    bool children_intersect = false;
    for ( int i=0; i<8; i++ )
    {
        const int ind = this_node->children[i];
        const NarrowTreeSdfNode * child_node = &( this_node->tree->nodes_sdf_.ptrw()[ind] );
        const bool ch_intersects = collide_backward( se3_rel, child_node, pts, depths );
        children_intersect = children_intersect || ch_intersects;
    }

    return children_intersect;
}


bool NarrowTreePtsNode::collide_points( const NarrowTreeSdfNode * this_node, Vector<Vector3d> & pts, Vector<Vector3d> & depths ) const
{
    bool ret = false;
    const Pose se3_rel = tree->pose_w() / this_node->tree->pose_w();
	const Cube & this_cube = this_node->cube_;
    const int qty = ptInds.size();
    for ( int i=0; i<qty; i++ )
    {
        const int ind = ptInds.ptr()[i];
        const Vector3d & pt = tree->pts_.ptr()[ind];
        const Vector3d pt_local = (se3_rel.q * pt) + se3_rel.r;

		// SDF in the node only works for the points inside of the node.
		const bool pt_inside = this_cube.contains( pt_local );
		if ( !pt_inside )
			continue;

        Vector3d depth;
        const Float d = this_node->distance_for_this_node( pt_local, depth );
        if ( d < 0.0 )
        {
			const Float d2 = this_node->distance_for_this_node( pt_local, depth );
			// It's the displecement of object "a". So it is in the opposite direction.
			depth = -depth;
			// Convert to global ref. frame.
            const Pose pose = this_node->tree->pose_w();
			const Vector3d at_local = pt_local - (depth*0.5);
            const Vector3d pt_global = ( pose.q * at_local ) + pose.r;
            pts.push_back( pt_global );
            // Convert to global ref. frame.
            depth = pose.q * depth;
            depths.push_back( depth );

            ret = true;
        }
    }

    return ret;
}


static bool compute_mean_and_std( NarrowTreePtsNode & node, Vector3d & std_diag, Vector3d & std_off_diag );

bool NarrowTreePtsNode::compute_cube_optimized()
{
    Quaterniond Q;
    const int qty = ptInds.size();
    if ( qty > 1 )
    {
        Vector3d std_diag;
        Vector3d std_off_diag;
        const bool std_ok = compute_mean_and_std( *this, std_diag, std_off_diag );
        if ( !std_ok )
            return false;
        std::array<Float, 3> eval;
        std::array<std::array<Float, 3>, 3> evec;
        gte::SymmetricEigensolver3x3<Float> solver;
        const int ret = solver( std_diag.x_, std_off_diag.x_, std_off_diag.y_, std_diag.y_, std_off_diag.z_, std_diag.z_,
                                                        false, -1, eval, evec );
        if ( ret < 1 )
            return false;

        Matrix3d A;
        A.m00_ = evec[0][0];
        A.m11_ = evec[1][1];
        A.m22_ = evec[2][2];

        A.m01_ = evec[0][1];
        A.m02_ = evec[0][2];
        A.m12_ = evec[1][2];

        A.m10_ = evec[1][0];
        A.m20_ = evec[2][0];
        A.m21_ = evec[2][1];

        Q.FromRotationMatrix( A );
        Q.Normalize();
    }
    else
        Q = Quaterniond( 1.0, 0.0, 0.0, 0.0 );

    const Vector3d ex = Q * Vector3d( 1.0, 0.0, 0.0 );
    const Vector3d ey = Q * Vector3d( 0.0, 1.0, 0.0 );
    const Vector3d ez = Q * Vector3d( 0.0, 0.0, 1.0 );

    Float x_min, x_max, y_min, y_max, z_min, z_max;

    // Initialize with the very first point.
    {
        const int ind = ptInds.ptr()[0];
        const Vector3d v = tree->pts_.ptr()[ind] - cube_.center_0;
        x_min = x_max = ex.DotProduct( v );
        y_min = y_max = ey.DotProduct( v );
        z_min = z_max = ez.DotProduct( v );
    }

    for ( int i=0; i<qty; i++ )
    {
        const int ind = ptInds.ptr()[i];
        const Vector3d v = tree->pts_.ptr()[ind] - cube_.center;
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

    const Vector3d c( (x_min+x_max)/2.0, (y_min+y_max)/2.0, (z_min+z_max)/2.0 );
    const Vector3d center = cube_.center + ex*c.x_ + ey*c.y_ + ez*c.z_;

    Float szx2 = (x_max - x_min)/2.0;
    if ( szx2 < EPS )
        szx2 = EPS;
    Float szy2 = (y_max - y_min)/2.0;
    if ( szy2 < EPS )
        szy2 = EPS;
    Float szz2 = (z_max - z_min)/2.0;
    if ( szz2 < EPS )
        szz2 = EPS;

    cube_optimized_.init( center, szx2, szy2, szz2 );
    Pose se3;
    se3.q = Q;
    cube_optimized_.apply( se3, true );

    return false;
}








static bool compute_mean_and_std( NarrowTreePtsNode & node, Vector3d & std_diag, Vector3d & std_off_diag )
{
    const int qty = node.ptInds.size();
    if ( qty <= 0 )
        return false;

    Vector3d m = Vector3d( 0.0, 0.0, 0.0 );
    for ( int i=0; i<qty; i++ )
    {
        const int ind = node.ptInds.ptr()[i];
        const Vector3d v = node.tree->pts_.ptr()[ind];
        m += v;
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
        const Vector3d & vd = node.tree->pts_.ptr()[ind];
        const Vector3d v = vd - m;
        std.m00_ += v.x_*v.x_;
        std.m11_ += v.y_*v.y_;
        std.m22_ += v.z_*v.z_;
        std.m01_ += v.x_*v.y_;
        std.m02_ += v.x_*v.z_;
        std.m12_ += v.y_*v.z_;
    }
    std.m00_ *= inv_qty;
    std.m11_ *= inv_qty;
    std.m22_ *= inv_qty;
    std.m01_ *= inv_qty;
    std.m02_ *= inv_qty;
    std.m12_ *= inv_qty;

    std_diag.x_ = std.m00_;
    std_diag.y_ = std.m11_;
    std_diag.z_ = std.m22_;
    std_off_diag.x_ = std.m01_;
    std_off_diag.y_ = std.m02_;
    std_off_diag.z_ = std.m12_;

    return true;
}





}



