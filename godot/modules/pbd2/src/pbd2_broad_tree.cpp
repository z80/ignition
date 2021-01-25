
#include "pbd2_broad_tree.h"
#include "pbd2_simulation.h"
#include "pbd2_rigid_body.h"
#include "pbd2_collision_object.h"



namespace Pbd
{

static const Float EPS = 0.0001;

BroadTree::BroadTree()
    : simulation( nullptr )
{
    max_depth_ = 2;
    min_size_  = 0.0;
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
   contacts_.clear();
}

void BroadTree::subdivide( Simulation * sim, Float h )
{
    simulation = sim;
    // Before eerything else clear nodes.
    nodes_.clear();
    collision_objects_.clear();
    
    Vector<RigidBody *> & bodies = simulation->bodies;
    const int bodies_qty = bodies.size();
    if ( bodies_qty < 1 )
        return;

    bool initialized = false;
    Float x_min, x_max, y_min, y_max, z_min, z_max;
    // Initialize with the very first point.
    for ( int i=0; i<bodies_qty; i++ )
    {
        RigidBody * body = bodies.ptr()[i];
        const int collisions_qty = body->collision_objects.size();
        for ( int j=0; j<collisions_qty; j++ )
        {
            CollisionObject * co = body->collision_objects.ptrw()[j];
            const Vector3d c = co->center();
            const Float sz = co->size2( h );
            x_min = x_max = c.x_;
            y_min = y_max = c.y_;
            z_min = z_max = c.z_;
            min_size_ = sz;
            initialized = true;
            break;
        }
        if ( initialized )
            break;
    }
    if ( !initialized )
        return;

    BroadTreeNode root;

    for ( int i=0; i<bodies_qty; i++ )
    {
        RigidBody * body = bodies.ptrw()[i];
        const int collisions_qty = body->collision_objects.size();
        for ( int j=0; j<collisions_qty; j++ )
        {
            CollisionObject * co = body->collision_objects.ptrw()[j];
            if ( co == nullptr )
                continue;
            const Float sz = co->size2( h );
            // Put only body indices for the bodies who have collision objects.
            // And collision objects are valid.
            collision_objects_.push_back( co );
            const int co_ind = collision_objects_.size() - 1;
            root.ptInds.push_back( co_ind );
            // Determine dimensions.
            const Vector3d center = co->center();
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
            // Determine minimum node size.
            if ( (sz > 0.0) && ( (min_size_ > sz) || (min_size_ < 0.0) ) )
                min_size_ = sz;
        }
    }
    // Half of the object size.
    //min_size_ *= 0.5;

    const Vector3d c( (x_min+x_max)/2.0, (y_min+y_max)/2.0, (z_min+z_max)/2.0 );
    const Vector3d dims( (x_max-x_min)/2.0, (y_max-y_min)/2.0, (z_max-z_min)/2.0 );
    Float d = ( dims.x_ > dims.y_ ) ? dims.x_ : dims.y_;
    d = (d > dims.z_) ? d : dims.z_;

    // Just to have some gap ???
    // Not sure if it is needed.
    d *= 1.1;

    root.level = 0;
    root.tree = this;
    root.center = c;
    root.size2  = d;
    root.init();
    insert_node( root );

    root.subdivide( h );
    update_node( root );
}




const Vector<ContactPointBb> & BroadTree::contact_points( RigidBody * body, Float h, RigidBody * & body_b )
{
    contacts_.clear();
    collision_object_inds_.clear();

    Vector<CollisionObject *> & objects = body->collision_objects;
    const int body_objects_qty = objects.size();

    for ( int i=0; i<body_objects_qty; i++ )
    {
        CollisionObject * co = objects.ptrw()[i];
        const int ind = collision_objects_.find( co );
        select_for_one( body, co, h, collision_object_inds_ );
    }
    remove_duplicates( collision_object_inds_ );

    if ( collision_object_inds_.empty() )
        body_b = nullptr;
    else
    {
        const int ind = collision_object_inds_.ptr()[0];
        CollisionObject * co = collision_object( ind );
        body_b = co->rigid_body;
    }

    // Intersect all contacts of requested body with all potential collision objects found.
    const int other_objects_qty = collision_object_inds_.size();
    for ( int i=0; i<body_objects_qty; i++ )
    {
        CollisionObject * body_object = objects.ptr()[i];
        const Pose pose_a = body_object->pose_w();
        for ( int j=0; j<other_objects_qty; j++ )
        {
            const int ind = collision_object_inds_.ptr()[j];
            CollisionObject * other_object = collision_object( ind );
            const Pose pose_b = other_object->pose_w();
            {
                ats_.clear();
                depths_.clear();

                body_object->intersect( other_object, ats_, depths_ );
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
                    pt.body_b = body_b;
                    contacts_.push_back( pt );
                }
            }
        }
    }

    // Now intersect in the opposite direction and revert the depth.
    for ( int j=0; j<other_objects_qty; j++ )
    {
        const int ind = collision_object_inds_.ptr()[j];
        CollisionObject * other_object = collision_object( ind );
        const Pose pose_b = other_object->pose_w();
        for ( int i=0; i<body_objects_qty; i++ )
        {
            CollisionObject * body_object = objects.ptr()[i];
            const Pose pose_a = body_object->pose_w();
            {
                ats_.clear();
                depths_.clear();

                other_object->intersect( body_object, ats_, depths_ );
                const int qty = ats_.size();
                for ( int i=0; i<qty; i++ )
                {
                    const Vector3d & at  = ats_.ptr()[i];
                    const Vector3d depth = depths_.ptr()[i];
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
                    pt.body_b = body_b;
                    contacts_.push_back( pt );
                }
            }
        }

    }

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

bool BroadTree::select_for_one( RigidBody * body, CollisionObject * co, Float h, Vector<int> & inds )
{
    // IF wasn't subdivided (may be because no bodies have collision objects), return "true".
    if ( nodes_.empty() )
        return true;

    const Vector3d center = co->center();
    const Float size2 = co->size2( h );

    const BroadTreeNode & root = nodes_.ptr()[0];
    const bool ok = root.objects_inside( body, co, h, inds );

    return ok;
}

void BroadTree::remove_duplicates( Vector<int> & inds )
{
    const int qty = inds.size();
    if ( qty < 1 )
        return;
    
    // Going to keep only collisions with one rigid body.
    // All bodies are collided in a loop anyway, so all will be eventually processed.
    const int first_ind = inds.ptr()[0];
    CollisionObject * first_co = collision_object( first_ind );
    const RigidBody * first_rb = first_co->rigid_body;

    int removed_qty = 0;
    for ( int i=0; i<(qty-removed_qty-1); i++ )
    {
        const int pa = inds.ptr()[i];
        bool do_again = true;
        while ( do_again )
        {
            do_again = false;
            for ( int j=(i+1); j<(qty-removed_qty); j++ )
            {
                const int pb = inds.ptr()[j];
                CollisionObject * co = collision_object( pb );
                const RigidBody * rb = co->rigid_body;
                if ( (pa == pb) || (rb == first_rb) )
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
	inds.resize( qty - removed_qty );
}

CollisionObject * BroadTree::collision_object( int ind )
{
    const int qty = collision_objects_.size();
    if ( ( ind < 0 ) || (ind >= qty) )
        return nullptr;

    CollisionObject * ret = collision_objects_.ptrw()[ind];
    return ret;
}






}
















