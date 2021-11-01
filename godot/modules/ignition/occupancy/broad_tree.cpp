
#include "broad_tree.h"
#include "ref_frame_node.h"
#include 


namespace Pbd
{

static const Float EPS = 0.0001;
static const Float MIN_CONTACT_DIST = 0.001;

static void find_mesh_instances( RefFrameNode * node, Vector<OctreeMesh *> & instances );


BroadTree::BroadTree()
    : simulation( nullptr )
{
    ref_frame_phyiscs_ = nullptr;
    max_depth_         = 2;
    min_size_          = 0.0;
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

void BroadTree::subdivide( RefFrameNode * ref_frame_physics )
{
    ref_frame_physics_ = ref_frame_physics;
    // Before eerything else clear nodes.
    nodes_.clear();
    octree_meshes_.clear();

    // Search for all occurences of OctreeMeshGd instances.
    find_mesh_instances( ref_frame_physics, octree_meshes_ );
    
    const int meshes_qty = meshes.size();
    if ( meshes_qty < 1 )
        return;

    bool initialized = false;
    Float x_min, x_max, y_min, y_max, z_min, z_max;
    // Initialize with the very first point.
    for ( int i=0; i<meshes_qty; i++ )
    {
        OctreeMesh * mesh = octree_meshes_.ptr()[i];
        {
            const Vector3d c = mesh->origin();
            const Float sz = mesh->size2();
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

    for ( int i=0; i<meshes_qty; i++ )
    {
        OctreeMesh * mesh = octree_mesh_.ptrw()[i];
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





struct SortByBodyB
{
	bool operator()(const CollisionObjectsPair & a, const CollisionObjectsPair & b) const
	{
		return (a.body_b < b.body_b);
	}
};


Vector<CollisionObjectsPair> & BroadTree::find_collision_candidates( RigidBody * body, Float h )
{
	contacts_.clear();
	collision_pairs_.clear();

	Vector<CollisionObject *> & objects = body->collision_objects;
	const int body_objects_qty = objects.size();

	for ( int i=0; i<body_objects_qty; i++ )
	{
		CollisionObject * co = objects.ptrw()[i];
		const int ind = collision_objects_.find( co );
		select_for_one( body, co, h, collision_object_inds_ );
		remove_duplicates( collision_object_inds_ );
		remove_if_disabled( body, collision_object_inds_ );

		const int objs_qty = collision_object_inds_.size();
		for ( int j=0; j<objs_qty; j++ )
		{
			const int obj_b_ind = collision_object_inds_.ptr()[j];
			CollisionObjectsPair pair;
			pair.co_a = ind;
			pair.co_b = obj_b_ind;
			const CollisionObject * co_b = collision_objects_.ptr()[obj_b_ind];
			pair.body_b = co_b->rigid_body;
			collision_pairs_.push_back( pair );
		}
	}

	// Need to sort collision pairs by body_b.
	collision_pairs_.sort_custom<SortByBodyB>();

	return collision_pairs_;
}

const Vector<ContactPointBb> & BroadTree::find_collision_points( Vector<CollisionObjectsPair> & pairs, int start_ind, int & end_ind )
{
	contacts_.clear();
	const int pairs_qty = pairs.size();
	if ( start_ind >= pairs_qty )
	{
		end_ind = pairs_qty;
		return contacts_;
	}

	// Current body "b".
	CollisionObjectsPair & pair_start = pairs.ptrw()[start_ind];
	const RigidBody * body_start = pair_start.body_b;
	const RigidBody * body_current = body_start;
	int index = start_ind;
	while (body_current == body_start)
	{
		CollisionObjectsPair & pair = pairs.ptrw()[index];

		// Collide "a" with "b" and "b" with "a".
		CollisionObject * body_object_a = collision_objects_.ptrw()[pair.co_a];
		RigidBody * body_a = body_object_a->rigid_body;
		const Pose pose_a = body_object_a->pose_w();

		CollisionObject * body_object_b = collision_objects_.ptr()[pair.co_b];
		RigidBody * body_b = body_object_b->rigid_body;
		const Pose pose_b = body_object_b->pose_w();

		ats_.clear();
		depths_.clear();

		body_object_a->intersect( body_object_b, ats_, depths_ );
		const int qty_a = ats_.size();
		for ( int i=0; i<qty_a; i++ )
		{
			const Vector3d & at    = ats_.ptr()[i];
			const Vector3d & depth = depths_.ptr()[i];
			const Float L          = depth.Length();
			if ( L < EPS )
				continue;

			ContactPointBb pt;
			pt.depth   = L;
			pt.n_world = depth / L;
			pt.r_a = pose_a.q.Inverse() * (at - pose_a.r);
			pt.r_b = pose_b.q.Inverse() * (at - pose_b.r);
			pt.body_a = body_a;
			pt.body_b = body_b;
			pt.pose_a = body_a->pose;
			pt.pose_b = body_b->pose;
			contacts_.push_back( pt );
		}

		ats_.clear();
		depths_.clear();

		body_object_b->intersect( body_object_a, ats_, depths_ );
		const int qty_b = ats_.size();
		for ( int i=0; i<qty_b; i++ )
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
			pt.r_a     = pose_a.q.Inverse() * (at - pose_a.r);
			pt.r_b     = pose_b.q.Inverse() * (at - pose_b.r);
			pt.body_a  = body_a;
			pt.body_b  = body_b;
			pt.pose_a  = body_a->pose;
			pt.pose_b  = body_b->pose;
			contacts_.push_back( pt );
		}


		// Increment index and get the next body.
		index += 1;
		if ( index >= pairs_qty )
		{
			end_ind = pairs_qty;

			// Remove spatial duplicates.
			remove_spatial_duplicates( contacts_ );

			return contacts_;
		}
		const CollisionObjectsPair & pair_current = pairs.ptr()[start_ind];
		body_current = pair_current.body_b;
	}

	// Remove spatial duplicates.
	remove_spatial_duplicates( contacts_ );

	end_ind = index;
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
	inds.clear();
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

void BroadTree::remove_if_disabled( RigidBody * body, Vector<int> & inds )
{
	const int joints_qty = body->joints.size();
	int removed_qty = 0;

	const int objs_qty = inds.size();

	for ( int i=0; i<joints_qty; i++ )
	{
		Joint * joint = body->joints.ptrw()[i];
		const bool disabled = joint->get_ignore_collisions();
		if ( !disabled )
			continue;

		bool redo = true;
		int start_ind = 0;
		while ( redo )
		{
			redo = false;
			for ( int j=start_ind; j<(objs_qty-removed_qty); j++ )
			{
				const int obj_ind = inds.ptr()[j];
				CollisionObject * o = collision_objects_.ptrw()[obj_ind];
				RigidBody * obj_body = o->rigid_body;
				if ( (obj_body == joint->body_a) || (obj_body == joint->body_b) )
				{
					const int obj_last_ind = objs_qty - removed_qty - 1;
					// Swap with the last element.
					if ( j != obj_last_ind )
					{
						const int ind_last = inds.ptr()[obj_last_ind];
						inds.ptrw()[j] = ind_last;
						inds.ptrw()[obj_last_ind] = obj_ind;
					}
					removed_qty += 1;
					start_ind = j;
					redo = true;
					break;
				}
			}
		}
	}

	inds.resize( objs_qty-removed_qty );
}

void BroadTree::remove_spatial_duplicates( Vector<ContactPointBb> & pts )
{
	const int qty = pts.size();
	if ( qty < 1 )
		return;

	// Removing points which are too close.
	// It usually means "a" collided with "b",
	// "b" collided with "a" and it is actually one and the same point.

	int removed_qty = 0;
	for ( int i=0; i<(qty-removed_qty-1); i++ )
	{
		const ContactPointBb pa = pts.ptr()[i];
		bool do_again = true;
		while ( do_again )
		{
			do_again = false;
			for ( int j=(i+1); j<(qty-removed_qty); j++ )
			{
				const ContactPointBb & pb = pts.ptr()[j];
				// Check if it is for the same pair of bodies.
				const bool same_bodies_straight = ( pa.body_a == pb.body_a ) && (pa.body_b == pb.body_b);
				const bool same_bodies_cross = ( pa.body_a == pb.body_b ) && (pa.body_b == pb.body_a);
				const bool same_bodies = same_bodies_straight || same_bodies_cross;
				// Measure distance. Let it be on body "a"
				bool remove = false;
				if ( !same_bodies )
					continue;
				const Vector3d dr = same_bodies_straight ? (pa.r_a - pb.r_a) : (pa.r_a - pb.r_b);
				const Float dist = dr.Length();
				if ( dist < MIN_CONTACT_DIST )
				{
					// Swap with the last one and increment "removed_qty".
					const int last_ind = qty - removed_qty - 1;
					const ContactPointBb last = pts.ptr()[last_ind];
					pts.ptrw()[last_ind] = pb;
					pts.ptrw()[j] = last;
					removed_qty += 1;
					do_again = true;
					break;
				}
			}
		}
	}
	pts.resize( qty - removed_qty );
}

CollisionObject * BroadTree::collision_object( int ind )
{
    const int qty = collision_objects_.size();
    if ( ( ind < 0 ) || (ind >= qty) )
        return nullptr;

    CollisionObject * ret = collision_objects_.ptrw()[ind];
    return ret;
}







static void find_mesh_instances( RefFrameNode * node, Vector<OctreeMesh *> & instances )
{
    const int children_qty = node->get_child_count();
    for ( int i=0; i<children_qty; i++ )
    {
        Node * ch = node->get_child( i );
        RefFrameNode * ref_frame = Node::cast_to<RefFrameNode>( ch );
        if (ref_frame == nullptr)
            continue;
        const int qty = ch->get_child_count();
        for ( int j=0; j<qty; j++ )
        {
            Node * cch = ch->get_child( j );
            OctreeMeshGd * octree_mesh = Node::cast_to<OctreeMesh>();
            if ( octree_mesh != nullptr )
            {
                const Vector3 at = ref_frame->r();
                const Quat    q  = ref_frame->q();
                octree_mesh->set_origin( at );
                octree_mesh->set_quat( q );
                instances.push_back( octree_mesh );
                break;
            }
        }
    }
}




}
















