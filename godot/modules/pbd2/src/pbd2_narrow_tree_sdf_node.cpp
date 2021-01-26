
#include "pbd2_narrow_tree_sdf_node.h"
#include "pbd2_narrow_tree.h"
#include "pbd2_narrow_tree_pts_node.h"
#include "matrix3d.h"

#include "SymmetricEigensolver3x3.h"

namespace Pbd
{

static const Float EPS = 0.0001;


NarrowTreeSdfNode::NarrowTreeSdfNode()
{
    this->tree = nullptr;
    parentAbsIndex = -1;
    indexInParent = -1;
    level = -1;
    absIndex = -1;

    for ( int i=0; i<8; i++ )
        children[i] = -1;

    on_or_below_surface = false;
    size2 = 1.0;
    center = Vector3( 0.0, 0.0, 0.0 );

    init();
    reset_distances();
}

NarrowTreeSdfNode::~NarrowTreeSdfNode()
{

}

NarrowTreeSdfNode::NarrowTreeSdfNode( const NarrowTreeSdfNode & inst )
{
    *this = inst;
}

const NarrowTreeSdfNode & NarrowTreeSdfNode::operator=( const NarrowTreeSdfNode & inst )
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

        on_or_below_surface = inst.on_or_below_surface;
        size2 = inst.size2;
        center = inst.center;
        
        cube_ = inst.cube_;

        d000 = inst.d000;
        d100 = inst.d100;
        d110 = inst.d110;
        d010 = inst.d010;
        d001 = inst.d001;
        d101 = inst.d101;
        d111 = inst.d111;
        d011 = inst.d011;

        init();
    }

    return *this;
}


bool NarrowTreeSdfNode::hasChildren() const
{
    for ( int i=0; i<8; i++ )
    {
        if ( children[i] >= 0 )
            return true;
    }

    return false;
}

bool NarrowTreeSdfNode::subdivide()
{
    // Self initialize distances.
    init_distances();
    // This one is needed only for leaf nodes,
    // but just to not keep value uninitialized.
    compute_on_or_below_surface();

    //If has reached max depth, stop right here.
    if ( level >= tree->max_depth_ )
    {
        return false;
    }
    
    // It can't have children because subdivision is happening.
    // But just in case.
    bool ch = hasChildren();
    if ( ch )
        return false;

    // Compute mid-points and depths.
    Vector3d mid_pts[7];
    Float    mid_depths[7];
    probe_distances( mid_pts, mid_depths );

    // If didn't reach minimum level, subdivide unconditionally.
    // If reached minimum level, subdivide if distance error is
    // bigger than the threshold specified.
    // Also, if it doesn't contain surface, there is really no need
    // to subdivide anyway.
    if ( this->level >= tree->min_depth_ )
    {
        // If it doesn't contain surface, nobody cares if it
        // isn't precise enough.
        const bool is_surface_node = contains_surface();
        if ( !is_surface_node )
            return false;

        // Check for depth error.
        const Float err = depth_error( mid_pts, mid_depths );
        if ( err < tree->max_sdf_error_ )
        {
            return false;
        }
    }

    const int childLevel = this->level + 1;
    const Float chSize2  = this->size2 * 0.5;

    NarrowTreeSdfNode nn[8];
    int    qtys[8];
    NarrowTreeSdfNode n;
    for ( int i=0; i<8; i++ )
    {
        n.absIndex = tree->insert_node_sdf( n );
        nn[i] = tree->nodes_sdf_[ n.absIndex ];

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

    // Subdivide recursively.
    for ( int i=0; i<8; i++ )
    {
        NarrowTreeSdfNode & ch_n = nn[i];
        ch_n.subdivide();
        tree->update_node_sdf( ch_n );
    }

    return true;
}

void NarrowTreeSdfNode::init()
{
    cube_.init( center, size2, size2, size2 );
}








bool NarrowTreeSdfNode::collide_forward( const Pose & se3_rel, const NarrowTreePtsNode * n, Vector<Vector3d> & pts, Vector<Vector3d> & depths ) const
{
    // Apply transforms.
    Cube other_cube = n->cube_;
    other_cube.apply( se3_rel );

    const bool intersects = cube_.intersects( other_cube );
    if ( !intersects )
        return false;
    const bool has_ch = n->hasChildren();
    if ( !has_ch )
    {
        // Check if the node contains points.
        const bool other_is_empty = n->ptInds.empty();
        if ( other_is_empty )
                return false;

        // Now the other leaf node contains 3d points. Need to make sure that
        // this node intersects a node in this SDF node which is on or below the surface.
        const Pose backward_rel_se3 = tree->pose_w() / n->tree->pose_w();
        const bool ret = n->collide_backward( backward_rel_se3, this, pts, depths );
        return ret;
    }

    bool children_intersect = false;
    for ( int i=0; i<8; i++ )
    {
        const int ind = n->children[i];
        const NarrowTreePtsNode * child_node = &( n->tree->nodes_pts_.ptr()[ind] );
        const bool ch_intersects = collide_forward( se3_rel, child_node, pts, depths );
        children_intersect = children_intersect || ch_intersects;
    }

    return children_intersect;
}


void NarrowTreeSdfNode::reset_distances()
{
    d000 = d100 = d110 = d010 = 
    d001 = d101 = d111 = d011 = -1.0;
}

void NarrowTreeSdfNode::init_distances()
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
            //const bool inside_mesh = point_inside_mesh( v );
            //if ( inside_mesh )
            //      int aaa = 0;
            // Compute the smallest distance to the surface.
            const Float min_dist = probe_distance( v );

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
}

static void probing_points( const Cube & c, Vector3d * pts )
{
    const Vector3d * vs = c.verts;
    // Bottom
    pts[0] = (vs[0] + vs[1] + vs[2] + vs[3])*0.25;
    // Top
    pts[1] = (vs[4] + vs[5] + vs[6] + vs[7])*0.25;
    // Going around.
    pts[2] = (vs[0] + vs[1] + vs[4] + vs[5])*0.25;
    pts[3] = (vs[1] + vs[2] + vs[5] + vs[6])*0.25;
    pts[4] = (vs[2] + vs[3] + vs[6] + vs[7])*0.25;
    pts[5] = (vs[3] + vs[0] + vs[7] + vs[4])*0.25;
    // Center.
    pts[6] = (pts[0] + pts[1]) * 0.5;
}

void NarrowTreeSdfNode::probe_distances( Vector3d * pts, Float * ds )
{
    probing_points( cube_, pts );
    for ( int i=0; i<7; i++ )
    {
        const Vector3d & v = pts[i];
        ds[i] = probe_distance( v );
    }
}

Float NarrowTreeSdfNode::probe_distance( const Vector3d & at ) const
{
    Vector3d displacement;
    const Float ret = probe_distance( at, displacement );
    return ret;
}

Float NarrowTreeSdfNode::probe_distance( const Vector3d & at, Vector3d & displacement ) const
{
    Float min_dist = -1.0;
    Vector3d min_displacement;

    const bool inside = point_inside_mesh( at );
    const int faces_qty = tree->faces_.size();
    for ( int j=0; j<faces_qty; j++ )
    {
        const Face & f = tree->faces_.ptr()[j];
        Vector3d disp;
        const Float d = f.distance( at, disp );
        if ( (min_dist < 0.0) || (min_dist > d) )
        {
            min_dist = d;
            min_displacement = disp;
        }
    }
    // If point is inside the mesh, point is by definition negative.
    if ( inside )
        min_dist = -min_dist;

    displacement = min_displacement;
    return min_dist;
}

Float NarrowTreeSdfNode::depth_error( Vector3d * pts, Float * ds )
{
    Float max_err = 0.0;
    for ( int i=0; i<7; i++ )
    {
        const Float real_d = ds[i];
        Vector3d disp;
        const Vector3d & v = pts[i];
        const Float d = distance_for_this_node( v, disp );
        const Float err = std::abs( real_d - d );
        if ( max_err < err )
            max_err = err;
    }

    return max_err;
}

void NarrowTreeSdfNode::generate_surface_points()
{
    const bool has_children = hasChildren();
    if ( has_children )
    {
        for ( int i=0; i<8; i++ )
        {
            const int ind = children[i];
            NarrowTreeSdfNode & child_node = tree->nodes_sdf_.ptrw()[ind];
            child_node.generate_surface_points();
        }
        return;
    }

    const Vector3d * vs = cube_.verts;
    Vector3d pts[8];
    pts[0] = (vs[0] + vs[6]*2.0)/3.0;
    pts[1] = (vs[1] + vs[7]*2.0)/3.0;
    pts[2] = (vs[2] + vs[4]*2.0)/3.0;
    pts[3] = (vs[3] + vs[5]*2.0)/3.0;
    
    pts[4] = (vs[0]*2.0 + vs[6])/3.0;
    pts[5] = (vs[1]*2.0 + vs[7])/3.0;
    pts[6] = (vs[2]*2.0 + vs[4])/3.0;
    pts[7] = (vs[3]*2.0 + vs[5])/3.0;
    for ( int i=0; i<8; i++ )
    {
        Vector3d adj;
        probe_distance( pts[i], adj );
        pts[i] += adj;

        tree->pts_.push_back( pts[i] );
    }
}

bool NarrowTreeSdfNode::distance( const Vector3d & r, Float & d, Vector3d & dist ) const
{
    Vector3d rel_r;
    const bool ok = distance_recursive( r, d, rel_r );
    if ( !ok )
        return false;
    dist = tree->se3_.q_ * rel_r;
    return true;
}

bool NarrowTreeSdfNode::distance_recursive( const Vector3d & r, Float & d, Vector3d & ret ) const
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
        const NarrowTreeSdfNode & child_node = tree->nodes_sdf_.ptr()[ch_ind];
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

Float NarrowTreeSdfNode::distance_for_this_node( const Vector3d & r, Vector3d & disp ) const
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


bool NarrowTreeSdfNode::point_inside_mesh( const Vector3d & r ) const
{
    static const int QTY = 3;
    static const Vector3d dirs[] = { Vector3d(1.0, 0.0, 0.0), Vector3d(0.0, 1.0, 0.0), Vector3d(0.0, 0.0, 1.0) };
    const NarrowTreeSdfNode & n0 = tree->nodes_sdf_.ptr()[0];
    const Float max_d = 4.0 * n0.size2;

    for ( int k=0; k<QTY; k++ )
    {
        const Vector3d r2 = r + dirs[k]*max_d;

        const int faces_qty = tree->faces_.size();
        int intersections_qty = 0;
        for ( int i=0; i<3; i++ )
        {
            const Face & tri = tree->faces_.ptr()[i];
            Vector3d at;
            bool concerning;
            const bool intersects = tri.intersects_eps( r, r2, EPS, at, concerning );
            if ( concerning && (i<QTY) )
                continue;
            if ( intersects )
                intersections_qty += 1;
        }
        const bool inside = ( (intersections_qty & 1) != 0 );
        return inside;
    }

    return false;
}

bool NarrowTreeSdfNode::point_inside( const Vector3d & at ) const
{
    const bool ret = cube_.contains( at );
    return ret;
}

bool NarrowTreeSdfNode::intersects_ray( const Vector3d & r1, const Vector3d & r2 ) const
{
    // Check bounding boxes.
    const bool ret = cube_.intersects( r1, r2 );
    if ( !ret )
        return false;
    const bool has_children = hasChildren();
    if ( !has_children )
        return true;

    for ( int i=0; i<8; i++ )
    {
        const int ch_ind = children[i];
        const NarrowTreeSdfNode & child_node = tree->nodes_sdf_.ptr()[ch_ind];
        const bool ret = child_node.intersects_ray( r1, r2 );
        if ( ret )
            return true;
    }
    return false;
}

bool NarrowTreeSdfNode::contains_surface() const
{
    int qty_under = 0;
    int qty_over  = 0;

    if ( d000 < 0.0 )
        qty_under += 1;
    else
        qty_over += 1;

    if ( d100 < 0.0 )
        qty_under += 1;
    else
        qty_over += 1;

    if ( d110 < 0.0 )
        qty_under += 1;
    else
        qty_over += 1;

    if ( d010 < 0.0 )
        qty_under += 1;
    else
        qty_over += 1;




    if ( d001 < 0.0 )
        qty_under += 1;
    else
        qty_over += 1;

    if ( d101 < 0.0 )
        qty_under += 1;
    else
        qty_over += 1;

    if ( d111 < 0.0 )
        qty_under += 1;
    else
        qty_over += 1;

    if ( d011 < 0.0 )
        qty_under += 1;
    else
        qty_over += 1;

    const bool ret = (qty_under > 0) && (qty_over > 0);
    return ret;
}

bool NarrowTreeSdfNode::is_above() const
{
    int qty = 0;

    if ( d000 >= 0.0 )
        qty += 1;

    if ( d100 >= 0.0 )
        qty += 1;

    if ( d110 >= 0.0 )
        qty += 1;

    if ( d010 >= 0.0 )
        qty += 1;




    if ( d001 >= 0.0 )
        qty += 1;

    if ( d101 >= 0.0 )
        qty += 1;

    if ( d111 >= 0.0 )
        qty += 1;

    if ( d011 >= 0.0 )
        qty += 1;

    const bool ret = (qty == 8);
    return ret;
}

bool NarrowTreeSdfNode::is_below() const
{
    int qty = 0;

    if ( d000 <= 0.0 )
        qty += 1;

    if ( d100 <= 0.0 )
        qty += 1;

    if ( d110 <= 0.0 )
        qty += 1;

    if ( d010 <= 0.0 )
        qty += 1;




    if ( d001 <= 0.0 )
        qty += 1;

    if ( d101 <= 0.0 )
        qty += 1;

    if ( d111 <= 0.0 )
        qty += 1;

    if ( d011 <= 0.0 )
        qty += 1;

    const bool ret = (qty == 8);
    return ret;
}


void NarrowTreeSdfNode::compute_on_or_below_surface()
{
    int qty_under = 0;

    if ( d000 < 0.0 )
        qty_under += 1;

    if ( d100 < 0.0 )
        qty_under += 1;

    if ( d110 < 0.0 )
        qty_under += 1;

    if ( d010 < 0.0 )
        qty_under += 1;




    if ( d001 < 0.0 )
        qty_under += 1;

    if ( d101 < 0.0 )
        qty_under += 1;

    if ( d111 < 0.0 )
        qty_under += 1;

    if ( d011 < 0.0 )
        qty_under += 1;

    on_or_below_surface = (qty_under > 0);
}










}



