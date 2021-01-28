
#include "pbd2_narrow_tree.h"
#include "pbd2_broad_tree.h"
#include "pbd2_broad_node.h"
#include "pbd2_rigid_body.h"
#include "pbd2_collision_utils.h"


namespace Pbd
{

static const Float EPS = 0.0001;

static void faces_from_surface( const Transform & t, const Mesh & mesh, int surface_idx, Vector<Face> & faces );
static void parse_mesh_arrays( const Transform & t, const Mesh & mesh, int surface_idx, bool is_index_array, Vector<Face> & faces );


NarrowTree::NarrowTree()
    : CollisionObject()
{
    obj_type = ObjectSdfMesh;

    max_sdf_error_ = 0.5;
    min_depth_ = 2;
    max_depth_ = 5;
    min_pts_   = 10;

    speed_margin = 0.0;
}

NarrowTree::~NarrowTree()
{
}

void NarrowTree::set_min_depth( int new_depth )
{
    min_depth_ = new_depth;
}

int NarrowTree::min_depth() const
{
    return min_depth_;
}

void NarrowTree::set_max_depth( int new_depth )
{
    max_depth_ = new_depth;
}

int NarrowTree::max_depth() const
{
    return max_depth_;
}

void NarrowTree::set_max_error( Float new_error )
{
    max_sdf_error_ = new_error;
}

Float NarrowTree::max_error() const
{
    return max_sdf_error_;
}

void NarrowTree::set_min_points( int new_qty )
{
    min_pts_ = new_qty;
}

int NarrowTree::min_points() const
{
    return min_pts_;
}

Float NarrowTree::bounding_radius() const
{
    const int qty = nodes_sdf_.size();
    if ( qty < 1 )
        return -1.0;
    const NarrowTreeSdfNode & n = nodes_sdf_.ptr()[0];
    const Float sz = n.size2;
    return sz;
}

bool NarrowTree::inside( const BroadTreeNode * n, Float h ) const
{
    Box box_other;
    box_other.init( Vector3d(n->size2, n->size2, n->size2) );
    Pose pose_other;
    pose_other.r = n->center;
    box_other.apply( pose_other );
    
    if ( nodes_sdf_.empty() )
        return false;

    const NarrowTreeSdfNode & root = nodes_sdf_.ptr()[0];
    
    Float sz = root.size2;
    if ( rigid_body )
    {
        const Float v = rigid_body->vel.Length();
        sz += k * v * h;
    }
    Box box_this;
    box_this.init( Vector3d(sz, sz, sz) );
    const Pose pose_this = pose_w();
    box_this.apply( pose_this );
    const bool ret = box_other.intersects( box_this );

    return ret;
}

void NarrowTree::clear()
{
    nodes_sdf_.clear();
    nodes_pts_.clear();
    faces_.clear();
    pts_.clear();
}

void NarrowTree::append( const Transform & t, const Ref<Mesh> & mesh )
{
    const int qty = mesh->get_surface_count();
    for ( int i=0; i<qty; i++ )
    {
        const Mesh & m = **mesh;
        faces_from_surface( t, m, i, faces_ );
    }
}

void NarrowTree::append_triangle( const Vector3d & a, const Vector3d & b, const Vector3d & c )
{
    Face f;
    f.init( a, b, c );
    faces_.push_back( f );
}

void NarrowTree::subdivide()
{
    if ( faces_.empty() )
        return;

    subdivide_sdf();
    /*{
            NarrowTreeSdfNode root_sdf = nodes_sdf_.ptrw()[0];
            merge_nodes_on_either_side( root_sdf );
    }*/
    {
            NarrowTreeSdfNode & root_sdf = nodes_sdf_.ptrw()[0];
            root_sdf.generate_surface_points();
    }
    remove_pt_duplicates();
    subdivide_pts();
}

void NarrowTree::subdivide_sdf()
{
    // Initialize faces.
    const int qty = faces_.size();
    for ( int i=0; i<qty; i++ )
    {
        Face & face = faces_.ptrw()[i];
        face.init();
    }

    Float x_max, y_max, z_max;
    // Initialize with the very first point.
    {
        const Face & f0 = faces_.ptr()[0];
        const Vector3d v0 = f0.verts_0[0];
        x_max = std::abs( v0.x_ );
        y_max = std::abs( v0.y_ );
        z_max = std::abs( v0.z_ );
    }

    const int faces_qty = faces_.size();
    for ( int i=0; i<faces_qty; i++ )
    {
        const Face & f = faces_.ptr()[i];
        for ( int j=0; j<3; j++ )
        {
            const Vector3d v = f.verts_0[j];
            const Float x = std::abs( v.x_ );
            const Float y = std::abs( v.y_ );
            const Float z = std::abs( v.z_ );
            if ( x > x_max )
                x_max = x;
            if ( y > y_max )
                y_max = y;
            if ( z > z_max )
                z_max = z;
        }
    }

    const Vector3d c( 0.0, 0.0, 0.0 );
    const Vector3d dims( x_max, y_max, z_max );
    Float d = ( dims.x_ > dims.y_ ) ? dims.x_ : dims.y_;
    d = (d > dims.z_) ? d : dims.z_;

    // Just to have some gap ???
    // Not sure if it is needed.
    d *= 1.1;

    nodes_sdf_.clear();
    NarrowTreeSdfNode root;
    root.level = 0;
    root.tree = this;
    root.center = c;
    root.size2  = d;
    root.init();
    insert_node_sdf( root );

    // Debugging. All the faces should be inside the root node.
    /*{
    int inside_qty = 0;
    for ( int i=0; i<qty; i++ )
    {
    const Face3 & f = faces_.ptr()[i];
    const bool inside = root.inside( f );
    if ( inside )
    inside_qty += 1;
    else
    {
    bool inside2 = root.inside( f );
    inside2 = inside2 || false;
    }
    }
    inside_qty += 0;
    }*/


    root.subdivide();
    update_node_sdf( root );
}

void NarrowTree::remove_pt_duplicates()
{
    const int qty = pts_.size();
    int removed_qty = 0;
    for ( int i=0; i<(qty-1); i++ )
    {
        bool removed = true;
        while ( removed )
        {
            removed = false;
            const Vector3d pt_a = pts_.ptr()[i];
            const int valid_qty = (qty - removed_qty);
            for ( int j=(i+1); j<valid_qty; j++ )
            {
                const Vector3d pt_b = pts_.ptr()[j];
                const Vector3d diff = pt_b - pt_a;
                const Float d = diff.Length();
                // Swap with the last element.
                if ( d < EPS )
                {
                    int last_ind = qty - removed_qty - 1;
                    const Vector3d last_pt = pts_.ptr()[last_ind];
                    pts_.ptrw()[last_ind] = pt_b;
                    pts_.ptrw()[j] = last_pt;
                    removed_qty += 1;

                    removed = true;
                }
            }
        }
    }

    pts_.resize( qty - removed_qty );
}

void NarrowTree::subdivide_pts()
{
    nodes_pts_.clear();
    NarrowTreePtsNode root;
    const NarrowTreeSdfNode & sdf_root = nodes_sdf_.ptr()[0];
    root.level = 0;
    root.tree = this;
    root.center = sdf_root.center;
    root.size2 = sdf_root.size2;
    root.init();

    root.ptInds.clear();
    const int pts_qty = pts_.size();
    for ( int i=0; i<pts_qty; i++ )
        root.ptInds.push_back( i );

    insert_node_pts( root );
    root.subdivide();
    update_node_pts( root );
}




void NarrowTree::intersect( CollisionObject * b, Vector<Vector3d> & ats, Vector<Vector3d> & depths )
{
    CollisionObjectType tp = b->type();
    if ( tp == ObjectSdfMesh )
    {
        NarrowTree * tree_b = dynamic_cast<NarrowTree *>( b );
        //intersect_sdf( tree_b, ats, depths );
		intersect_brute_force( tree_b, ats, depths );
    }
}

bool NarrowTree::intersect_sdf( NarrowTree * tree, Vector<Vector3d> & pts, Vector<Vector3d> & depths ) const
{
    if ( !tree )
        return false;

    const NarrowTreeSdfNode & root_sdf = nodes_sdf_.ptr()[0];
    const NarrowTreePtsNode * root_pts = &( tree->nodes_pts_.ptr()[0] );

    const Pose se3_rel = tree->pose_w() / this->pose_w();
    pts.clear();
    depths.clear();
    const bool ret = root_sdf.collide_forward( se3_rel, root_pts, pts, depths );

    return ret;
}

bool NarrowTree::intersect_brute_force( NarrowTree * tree_b, Vector<Vector3d> & pts, Vector<Vector3d> & depths )const
{
	if ( nodes_sdf_.empty() )
		return false;

	const NarrowTreeSdfNode & root = nodes_sdf_.ptr()[0];

	const Pose pose_a = pose_w();
	const Pose pose_b = tree_b->pose_w();
	const Pose pose_rel = pose_b / pose_a;
	const int pts_qty_b = tree_b->pts_.size();
	bool ret = false;
	for ( int i=0; i<pts_qty_b; i++ )
	{
		const Vector3d & pt_local = tree_b->pts_.ptr()[i];
		// Convert to current tree's ref frame.
		const Vector3d pt_rel = pose_rel.r + (pose_rel.q * pt_local);
		Vector3d at;
		Vector3d depth;
		const bool collides = root.point_collides( pt_rel, at, depth );
		if ( collides )
		{
			const Float d = depth.Length();
			print_line( String( "depth: " ) + rtos( d ) );

			depth = -depth;
			at = pose_a.r + (pose_a.q * at);
			depth = pose_a.q * depth;
			pts.push_back( at );
			depths.push_back( depth );
		}
		ret = ret || collides;
	}

	return ret;
}



PoolVector3Array NarrowTree::lines_sdf_nodes() const
{
    const Pose pose = pose_w();

    Vector<Vector3> ls;
    const int qty = nodes_sdf_.size();
    for ( int i=0; i<qty; i++ )
    {
        const NarrowTreeSdfNode & n = nodes_sdf_.ptr()[i];
        const bool has_ch = n.hasChildren();
        if ( has_ch )
            continue;

        const Vector3d * vs = n.cube_.verts;
        Vector3 v[8];
        for ( int i=0; i<8; i++ )
        {
            const Vector3d & v_local = vs[i];
            const Vector3d vd = pose.r + (pose.q * v_local);
            v[i] = Vector3( vd.x_, vd.y_, vd.z_ );
        }

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

PoolVector3Array NarrowTree::lines_pts_nodes() const
{
    const Pose pose = pose_w();

    Vector<Vector3> ls;
    const int qty = nodes_pts_.size();
    for ( int i=0; i<qty; i++ )
    {
        const NarrowTreePtsNode & n = nodes_pts_.ptr()[i];
        const bool has_ch = n.hasChildren();
        if ( has_ch )
            continue;
        if ( n.ptInds.empty() )
            continue;

        const Vector3d * vs = n.cube_.verts;
        Vector3 v[8];
        for ( int i=0; i<8; i++ )
        {
            const Vector3d & v_local = vs[i];
            const Vector3d vd = pose.r + (pose.q * v_local);
            v[i] = Vector3( vd.x_, vd.y_, vd.z_ );
        }

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

PoolVector3Array NarrowTree::lines_surface_pts() const
{
    const Pose pose = pose_w();

    const int qty = pts_.size();
    PoolVector3Array res;
    res.resize( qty );

    for ( int i=0; i<qty; i++ )
    {
        const Vector3d & v_local = pts_.ptr()[i];
        const Vector3d vd = pose.r + (pose.q * v_local);
        const Vector3 v( vd.x_, vd.y_, vd.z_ );
        res.set( i, v );
    }
    return res;
}

PoolVector3Array NarrowTree::lines_aligned_nodes() const
{
    const Pose pose = pose_w();

    Vector<Vector3> ls;
    const int qty = nodes_pts_.size();
    for ( int i=0; i<qty; i++ )
    {
        const NarrowTreePtsNode & n = nodes_pts_.ptr()[i];
        const bool has_ch = n.hasChildren();
        if ( has_ch )
            continue;
        if ( n.ptInds.empty() )
            continue;

        const Vector3d * vs = n.cube_optimized_.verts;
        Vector3 v[8];
        for ( int i=0; i<8; i++ )
        {
            const Vector3d & v_local = vs[i];
            const Vector3d vd = pose.r + (pose.q * v_local);
            v[i] = Vector3( vd.x_, vd.y_, vd.z_ );
        }

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


bool NarrowTree::parent_sdf( const NarrowTreeSdfNode & node, NarrowTreeSdfNode * & parent )
{
    if ( node.parentAbsIndex < 0 )
    {
        parent = 0;
        return false;
    }

    parent = &( nodes_sdf_.ptrw()[ node.parentAbsIndex ] );
    return true;
}

bool NarrowTree::parent_pts( const NarrowTreePtsNode & node, NarrowTreePtsNode * & parent )
{
    if ( node.parentAbsIndex < 0 )
    {
        parent = 0;
        return false;
    }

    parent = &( nodes_pts_.ptrw()[ node.parentAbsIndex ] );
    return true;
}

int  NarrowTree::insert_node_sdf( NarrowTreeSdfNode & node )
{
    nodes_sdf_.push_back( node );
    const int ind = static_cast<int>(nodes_sdf_.size()) - 1;
    NarrowTreeSdfNode * nns = nodes_sdf_.ptrw();
    NarrowTreeSdfNode & n = nns[ind];
    n.tree     = this;
    n.absIndex = ind;

    node = n;
    return ind;
}

int  NarrowTree::insert_node_pts( NarrowTreePtsNode & node )
{
    nodes_pts_.push_back( node );
    const int ind = static_cast<int>(nodes_pts_.size()) - 1;
    NarrowTreePtsNode * nns = nodes_pts_.ptrw();
    NarrowTreePtsNode & n = nns[ind];
    n.tree     = this;
    n.absIndex = ind;

    node = n;
    return ind;
}

void NarrowTree::update_node_sdf( const NarrowTreeSdfNode & node )
{
    nodes_sdf_.ptrw()[ node.absIndex ] = node;
}

void NarrowTree::update_node_pts( const NarrowTreePtsNode & node )
{
    nodes_pts_.ptrw()[ node.absIndex ] = node;
}

void NarrowTree::swap_nodes_sdf( int ind_a, int ind_b )
{
    NarrowTreeSdfNode a = nodes_sdf_.ptr()[ind_a];
    NarrowTreeSdfNode b = nodes_sdf_.ptr()[ind_b];
    {
        // update self, parent and children.
        b.absIndex = ind_a;
        const bool has_children = b.hasChildren();
        if ( has_children )
        {
            for ( int i=0; i<8; i++ )
            {
                const int ind = b.children[i];
                NarrowTreeSdfNode & c = nodes_sdf_.ptrw()[ind];
                c.parentAbsIndex = ind_a;
            }
            
            if ( b.parentAbsIndex >= 0 )
            {
                const int ind = b.parentAbsIndex;
                NarrowTreeSdfNode & p = nodes_sdf_.ptrw()[ind];
                p.children[ b.indexInParent ] = ind_a;
            }
        }
        nodes_sdf_.ptrw()[ind_a] = b;
    }

    {
        // update self, parent and children.
        a.absIndex = ind_b;
        const bool has_children = a.hasChildren();
        if ( has_children )
        {
            for ( int i=0; i<8; i++ )
            {
                const int ind = a.children[i];
                NarrowTreeSdfNode & c = nodes_sdf_.ptrw()[ind];
                c.parentAbsIndex = ind_b;
            }

            if ( a.parentAbsIndex >= 0 )
            {
                const int ind = a.parentAbsIndex;
                NarrowTreeSdfNode & p = nodes_sdf_.ptrw()[ind];
                p.children[ a.indexInParent ] = ind_b;
            }
        }
        nodes_sdf_.ptrw()[ind_a] = b;

        nodes_sdf_.ptrw()[ind_b] = a;
    }
}

void NarrowTree::remove_node_sdf( int ind )
{
    NarrowTreeSdfNode n = nodes_sdf_.ptr()[ind];

    if ( n.parentAbsIndex >= 0 )
    {
        NarrowTreeSdfNode & p = nodes_sdf_.ptrw()[n.parentAbsIndex];
        p.children[n.indexInParent] = -1;
    }

    const bool has_children = n.hasChildren();
    if ( has_children )
    {
        for ( int i=0; i<8; i++ )
        {
            const int child_ind = n.children[i];
            remove_node_sdf( child_ind );
        }
    }

    const int last_ind = nodes_sdf_.size() - 1;
    if ( ind != last_ind )
    {
        swap_nodes_sdf( ind, last_ind );
    }

    nodes_sdf_.resize( last_ind );
}

void NarrowTree::merge_nodes_on_either_side( NarrowTreeSdfNode n )
{
    const bool has_children = n.hasChildren();
    if ( !has_children )
        return;

    if ( n.level > min_depth_ )
    {
        int qty_above = 0;
        int qty_below = 0;
        for ( int i=0; i<8; i++ )
        {
            const int child_ind = n.children[i];
            const NarrowTreeSdfNode & cn = nodes_sdf_.ptr()[child_ind];
            const bool is_above = cn.is_above();
            if ( is_above )
                qty_above += 1;
            else
            {
                const bool is_below = cn.is_below();
                if ( is_below )
                    qty_below += 1;
            }
        }
        if ( (qty_above == 8) || (qty_below == 8) )
        {
            for ( int i=0; i<8; i++ )
            {
                const int ind = 7-i;
                const int child_ind = n.children[ind];
                remove_node_sdf( child_ind );
            }
            return;
        }
    }

    for ( int i=0; i<8; i++ )
    {
        const int child_ind = n.children[i];
        const NarrowTreeSdfNode cn = nodes_sdf_.ptr()[child_ind];
        merge_nodes_on_either_side( cn );
    }
}
















static void faces_from_surface( const Transform & t, const Mesh & mesh, int surface_idx, Vector<Face> & faces )
{
    // Don't add faces if doesn't consist of triangles.
    if (mesh.surface_get_primitive_type(surface_idx) != Mesh::PRIMITIVE_TRIANGLES)
        return;

    if ( mesh.surface_get_format(surface_idx) & Mesh::ARRAY_FORMAT_INDEX )
    {
        return parse_mesh_arrays( t, mesh, surface_idx, true, faces );
    }
    else
    {
        return parse_mesh_arrays( t, mesh, surface_idx, false, faces );
    }
}

static void parse_mesh_arrays( const Transform & t, const Mesh & mesh, int surface_idx, bool is_index_array, Vector<Face> & faces )
{
    const int vert_count = is_index_array ? mesh.surface_get_array_index_len( surface_idx ) :
        mesh.surface_get_array_len( surface_idx );
    if (vert_count == 0 || vert_count % 3 != 0)
        return;

    const int d_qty = vert_count / 3;
    const int orig_qty = faces.size();
    faces.resize( orig_qty + d_qty );

    Array arrays = mesh.surface_get_arrays( surface_idx );
    //FaceFiller filler(faces, arrays);

    PoolVector<int> indices = arrays[Mesh::ARRAY_INDEX];
    PoolVector<int>::Read indices_reader = indices.read();

    PoolVector<Vector3> vertices = arrays[Mesh::ARRAY_VERTEX];
    PoolVector<Vector3>::Read vertices_reader = vertices.read();

    if ( is_index_array )
    {
        for (int i = 0; i < vert_count; i++)
        {
            const int face_idx   = i / 3;
            const int set_offset = i % 3;
            const int lookup_index = indices_reader[i];
            const Vector3 v = vertices_reader[lookup_index];
            const Vector3 vt = t.xform( v );
            Face & face = faces.ptrw()[ orig_qty + face_idx ];
            face.verts_0[set_offset] = Vector3d( vt.x, vt.y, vt.z );
        }
    }
    else
    {
        for (int i = 0; i < vert_count; i++)
        {
            const int face_idx   = i / 3;
            const int set_offset = i % 3;
            const int lookup_index = i;
            const Vector3 v = vertices_reader[lookup_index];
            const Vector3 vt = t.xform( v );
            Face & face = faces.ptrw()[ orig_qty + face_idx ];
            face.verts_0[set_offset] = Vector3d( vt.x, vt.y, vt.z );
        }
    }
}



}




