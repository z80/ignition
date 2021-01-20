
#include "pbd2_broad_tree.h"
#include "pbd2_narrow_tree.h"


namespace Pbd
{


static void faces_from_surface( const Transform & t, const Mesh & mesh, int surface_idx, Vector<Face> & faces );
static void parse_mesh_arrays( const Transform & t, const Mesh & mesh, int surface_idx, bool is_index_array, Vector<Face> & faces );


BroadTree::BroadTree()
{
    max_depth_ = 5;
}

BroadTree::~BroadTree()
{
}

void BroadTree::set_max_depth( int new_depth )
{
    max_depth_ = new_depth;
}

int BroadTree::max_depth() const
{
    return max_depth_;
}

void BroadTree::clear()
{
   nodes_.clear();
   bodies_.clear();
   contacts_.clear();
}

void BroadTree::subdivide()
{
    // Before eerything else clear nodes.
    nodes_.clear();


    Float x_min, x_max, y_min, y_max, z_min, z_max;
    // Initialize with the very first point.
    {
        const NarrowTree * nt = bodies_.ptr()[0];
        const Vector3d c = nt->center();
        const Float sz = nt->size2();
        x_min = x_max = c.x_;
        y_min = y_max = c.y_;
        z_min = z_max = c.z_;
    }

    const int bodies_qty = bodies_.size();
    for ( int i=0; i<bodies_qty; i++ )
    {
        const NarrowTree * nt = bodies_.ptr()[i];
        const Float sz = nt->size2();
        if ( sz < 0.0 )
            continue;
        const Vector3d center = nt->center();
        const Vector3d v_max = center + Vector3d( sz, sz, sz );
        const Vector3d v_min = center - Vector3d( sz, sz, sz );
        if ( v_min.x_ < x_min )
            x_min = v_min.x_;
        if ( v_max.x_ > x_max )
            x_max = v_max.x_;
        if ( v_min.y_ < y_min )
            y_min = v_min.y_;
        if ( v_max.y_ > y_max )
            y_max = v_max.y_;
        if ( v_min.z_ < z_min )
            z_min = v_min.z_;
        if ( v_max.z_ > z_max )
            z_max = v_max.z_;
    }

    const Vector3d c( (x_min+x_max)/2.0, (y_min+y_max)/2.0, (z_min+z_max)/2.0 );
    const Vector3d dims( (x_max-x_min)/2.0, (y_max-y_min)/2.0, (z_max-z_min)/2.0 );
    Float d = ( dims.x_ > dims.y_ ) ? dims.x_ : dims.y_;
    d = (d > dims.z_) ? d : dims.z_;

    // Just to have some gap ???
    // Not sure if it is needed.
    d *= 1.1;

    nodes_.clear();
    BroadTreeNode root;
    root.level = 0;
    root.tree = this;
    root.center = c;
    root.size2  = d;
    root.init();
    for ( int i=0; i<bodies_qty; i++ )
        root.ptInds.push_back( i );
    insert_node( root );

    root.subdivide();
    update_node( root );
}




const Vector<int> & BroadTree::intersect_with_all( int ind )
{
    body_inds_.clear();
    select_for_one( ind, body_inds_ );
    remove_duplicates( body_inds_ );

    return body_inds_;
}

const Vector<Contact_pointBb> & BroadTree::contact_points( int ind_a, int ind_b )
{
    contacts_.clear();
    collide_pair( ind_a, ind_b );

    return contacts_;
}



PoolVector3Array BroadTree::lines_nodes() const
{
    Vector<Vector3> ls;
    const int qty = nodes_.size();
    for ( int i=0; i<qty; i++ )
    {
        const BroadTreeNode & n = nodes_.ptr()[i];
        const bool has_ch = n.hasChildren();
        if ( has_ch )
            continue;

        Vector3 v[8];
        const Vector3d cd = n.center;
        const Vector3 c( cd.x_, cd.y_, cd.z_ );
        const real_t sz = n.size2;
        v[0] = c + Vector3( -sz, -sz, -sz );
        v[1] = c + Vector3(  sz, -sz, -sz );
        v[2] = c + Vector3(  sz,  sz, -sz );
        v[3] = c + Vector3( -sz,  sz, -sz );
        v[4] = c + Vector3( -sz, -sz,  sz );
        v[5] = c + Vector3(  sz, -sz,  sz );
        v[6] = c + Vector3(  sz,  sz,  sz );
        v[7] = c + Vector3( -sz,  sz,  sz );

        ls.push_back( v[0] );
        ls.push_back( v[1] );

        ls.push_back( v[1] );
        ls.push_back( v[2] );

        ls.push_back( v[2] );
        ls.push_back( v[3] );

        ls.push_back( v[3] );
        ls.push_back( v[0] );


        ls.push_back( v[4] );
        ls.push_back( v[5] );

        ls.push_back( v[5] );
        ls.push_back( v[6] );

        ls.push_back( v[6] );
        ls.push_back( v[7] );

        ls.push_back( v[7] );
        ls.push_back( v[4] );


        ls.push_back( v[0] );
        ls.push_back( v[4] );

        ls.push_back( v[1] );
        ls.push_back( v[5] );

        ls.push_back( v[2] );
        ls.push_back( v[6] );

        ls.push_back( v[3] );
        ls.push_back( v[7] );
    }

    PoolVector3Array res;
    const int sz = ls.size();
    res.resize( sz );
    for ( int i=0; i<sz; i++ )
    {
        const Vector3 & v = ls.ptr()[i];
        res.set( i, v );
    }

    return res;
}


bool BroadTree::parent( const BroadTreeNode & node, BroadTreeNode * & parent )
{
    if ( node.parentAbsIndex < 0 )
    {
        parent = 0;
        return false;
    }

    parent = &( nodes_.ptrw()[ node.parentAbsIndex ] );
    return true;
}

int  BroadTree::insert_node( BroadTreeNode & node )
{
    nodes_.push_back( node );
    const int ind = static_cast<int>(nodes_.size()) - 1;
    BroadTreeNode * nns = nodes_.ptrw();
    BroadTreeNode & n = nns[ind];
    n.tree     = this;
    n.absIndex = ind;

    node = n;
    return ind;
}

void BroadTree::update_node( const BroadTreeNode & node )
{
    nodes_.ptrw()[ node.absIndex ] = node;
}

bool BroadTree::select_for_one( int tree_ind, Vector<int> & inds )
{
    inds.clear();
    const int qty = bodies_.size();
    if ( qty < 1 )
        return false;

    const int bodies_qty = bodies_.size();
    if ( tree_ind >= bodies_qty )
        return false;

    const NarrowTree * tree = bodies_.ptr()[tree_ind];
    const Vector3d center = tree->center();
    const Float size2 = tree->size2();

    const BroadTreeNode & root = nodes_.ptr()[0];
    const bool ok = root.objects_inside( tree_ind, center, size2, inds );

    return ok;
}

void BroadTree::remove_duplicates( Vector<int> & inds )
{
    const int qty = inds.size();
    int removed_qty = 0;
    for ( int i=0; i<(qty-removed_qty-1); i++ )
    {
        const int pa = inds.ptr()[i];
        bool do_again = true;
        while ( do_again )
        {
            do_again = false;
            for ( int j=(i+1); j<(qty-removed_qty); i++ )
            {
                const int pb = inds.ptr()[j];
                if ( pa == pb )
                {
                    // Swap with the last one and increment "removed_qty".
                    const int last_ind = qty - removed_qty - 1;
                    const int last = inds.ptr()[last_ind];
                    inds.ptrw()[last_ind] = pb;
                    inds.ptrw()[j] = last;
                    removed_qty += 1;
                    do_again = true;
                    break;
                }
            }
        }
    }
}

void BroadTree::collide_pair( int ind_a, int ind_b )
{
    NarrowTree * tree_a = bodies_.ptrw()[ind_a];
    NarrowTree * tree_b = bodies_.ptrw()[ind_b];
    const Pose pose_a = tree_a->pose();
    const Pose pose_b = tree_b->pose();

    // Intersect "a" with "b".
    {
        ats_.clear();
        depths_.clear();

        tree_a->intersects( tree_b, ats_, depths_ );
        const int qty = ats_.size();
        for ( int i=0; i<qty; i++ )
        {
            const Vector3d & at    = ats_.ptr()[i];
            const Vector3d & depth = depths_.ptr()[i];
            const Float L = depth.Length();
            if ( L < EPS )
                continue;

            ContactPointBb pt;
            pt.depth   = L;
            pt.n_world = depth / L;
            pt.r_a = pose_a.q.Inverse() * (at - pose_a.r);
            pt.r_b = pose_b.q.Inverse() * (at - pose_b.r);
            contacts_.push_back( pt );
        }
    }

    // Now the opposite. Intersect "b" with "a".
    {
        ats_.clear();
        depths_.clear();

        tree_b->intersects( tree_a, ats_, depths_ );
        const int qty = ats_.size();
        for ( int i=0; i<qty; i++ )
        {
            const Vector3d & at    = ats_.ptr()[i];
            const Float L = depth.Length();
            if ( L < EPS )
                continue;

            ContactPointBb pt;
            pt.depth   = L;
            // Here it is with "-". as here "b" collided with "a".
            // But depth is supposed to be towsrds "a".
            pt.n_world = -depth / L;
            pt.r_a = pose_a.q.Inverse() * (at - pose_a.r);
            pt.r_b = pose_b.q.Inverse() * (at - pose_b.r);
            collisions_.push_back( pt );
        }
    }
}






}
















