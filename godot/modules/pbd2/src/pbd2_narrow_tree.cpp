
#include "pbd2_narrow_tree.h"


namespace Pbd
{


static void faces_from_surface( const Transform & t, const Mesh & mesh, int surface_idx, Vector<Face> & faces );
static void parse_mesh_arrays( const Transform & t, const Mesh & mesh, int surface_idx, bool is_index_array, Vector<Face> & faces );


NarrowTree::NarrowTree()
{
	max_sdf_error_ = 0.1;
	min_depth_ = 1;
    max_depth_ = 3;
	min_pts_   = 10;
}

NarrowTree::~NarrowTree()
{
}

void NarrowTree::set_max_level( int new_level )
{
    max_depth_ = new_level;
}

int NarrowTree::max_level() const
{
    return max_depth_;
}

void NarrowTree::clear()
{
    nodes_sdf_.clear();
    faces_.clear();
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
	subdivide_sdf();
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

	Float x_min, x_max, y_min, y_max, z_min, z_max;
	// Initialize with the very first point.
	{
		const Face & f0 = faces_.ptr()[0];
		const Vector3d v0 = f0.verts_0[0];
		x_min = x_max = v0.x_;
		y_min = y_max = v0.y_;
		z_min = z_max = v0.z_;
	}

	const int faces_qty = faces_.size();
	for ( int i=0; i<faces_qty; i++ )
	{
		const Face & f = faces_.ptr()[i];
		for ( int j=0; j<3; j++ )
		{
			const Vector3d v = f.verts_0[j];
			const Float x = v.x_;
			const Float y = v.y_;
			const Float z = v.z_;
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

	const Vector3d c( (x_min+x_max)/2.0, (y_min+y_max)/2.0, (z_min+z_max)/2.0 );
	const Vector3d dims( (x_max-x_min)/2.0, (y_max-y_min)/2.0, (z_max-z_min)/2.0 );
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
    root.value  = qty;
    root.ptInds.clear();
    for ( int i=0; i<qty; i++ )
        root.ptInds.push_back( i );
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

	// Recursively compute distance fields.
	NarrowTreeSdfNode & n = nodes_sdf_.ptrw()[0];
	n.init_distances();
}

void NarrowTree::subdivide_pts()
{
	nodes_pts_.clear();
	NarrowTreePtsNode root;
	const NarrowTreeSdfNode & sdf_root = nodes_sdf_.ptr()[0];
	root.center = sdf_root.center;
	root.size2 = sdf_root.size2;

	const int pts_qty = pts_.size();
		for ( int i=0; i<pts_qty; i++ )
		root.ptInds.push_back( i );

	root.value = pts_qty;

    insert_node_pts( root );
	root.subdivide();
	update_node_pts( root );
}

void NarrowTree::apply( const SE3 & se3 )
{
	se3_ = se3;
}




bool NarrowTree::intersects( NarrowTree * tree, Vector<Vector3d> & pts, Vector<Vector3d> & depths ) const
{
    if ( !tree )
        return false;

    NarrowTreeSdfNode & root_sdf = nodes_sdf_.ptrw()[0];
	NarrowTreePtsNode * root_pts = &( tree->nodes_pts_.ptrw()[0] );

	const SE3 se3_rel = tree->se3_ / this->se3_;
	pts.clear();
	depths.clear();
	const bool ret = root_sdf.collide_forward( se3_rel, root_pts, pts, depths );

    return ret;
}



PoolVector3Array NarrowTree::lines_sdf_nodes()
{
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
			const Vector3d & vd = vs[i];
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

PoolVector3Array NarrowTree::lines_surface_pts()
{
	const int qty = pts_.size();
	PoolVector3Array res;
	res.resize( qty );

	for ( int i=0; i<qty; i++ )
	{
		const Vector3d & vd = pts_.ptr()[i];
		const Vector3 v( vd.x_, vd.y_, vd.z_ );
		res.set( i, v );
	}
	return res;
}

PoolVector3Array NarrowTree::lines_aligned_cubes()
{
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
			const Vector3d & vd = vs[i];
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




