
#include "pbd2_narrow_tree.h"


namespace Pbd
{


static void faces_from_surface( const Transform & t, const Mesh & mesh, int surface_idx, Vector<Face> & faces );
static void parse_mesh_arrays( const Transform & t, const Mesh & mesh, int surface_idx, bool is_index_array, Vector<Face> & faces );


NarrowTree::NarrowTree()
{
    // Initialize counters and parameters.
    this->max_depth_     = 1;
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
    nodes_.clear();
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
    // Initialize vertices.
    const int qty = faces_.size();
    for ( int i=0; i<qty; i++ )
    {
        Face & face = faces_.ptrw()[i];
        face.init();
    }
    // Get average of all the face points.
    Vector3d c = Vector3d( 0.0, 0.0, 0.0 );
    for ( int i=0; i<qty; i++ )
    {
        const Face & f = faces_.ptr()[i];
        c += f.verts[0];
        c += f.verts[1];
        c += f.verts[2];
    }
    const Float s = 1.0 / static_cast<Float>( qty );
    c *= s;

    // Here find the largest distance.
    Float d = 0.0;
    for ( int i=0; i<qty; i++ )
    {
        const Face & f = faces_.ptr()[i];
        for ( int j=0; j<3; j++ )
        {
            const Vector3d dv = f.verts[j] - c;
            const Float l = dv.Length();
            if ( l > d )
                d = l;
        }
    }

	// Just to have some gap ???
	// Not sure if it is needed.
    d *= 1.5;

    nodes_.clear();
    NarrowTreeNode root;
    root.level = 0;
    root.tree = this;
    root.center = c;
    root.size2  = d;
    root.value  = qty;
    root.ptInds.clear();
    for ( int i=0; i<qty; i++ )
        root.ptInds.push_back( i );
    root.init();
    insert_node( root );

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
    nodes_.ptrw()[ 0 ] = root;

	// Recursively compute distance fields.
	NarrowTreeNode & n = nodes_.ptrw()[0];
	n.init_distances();
}

void NarrowTree::apply( const SE3 & se3 )
{
	se3_ = se3;
}




bool NarrowTree::intersects( NarrowTree * tree )
{
    if ( !tree )
        return false;

    NarrowTreeNode & root = nodes_.ptrw()[0];

    const bool is_intersecting = node_intersects( root, *tree );

    return is_intersecting;
}

bool NarrowTree::node_intersects( NarrowTreeNode & n, NarrowTree & tree )
{
    NarrowTreeNode & other_root = tree.nodes_.ptrw()[0];
    const bool is_intersecting = n.inside( other_root );
    if ( !is_intersecting )
        return false;
    const bool has_ch = n.hasChildren();
    if ( !has_ch )
    {
        const bool is_filled = (n.value > 0);
        return is_filled;
    }

    for ( int i=0; i<8; i++ )
    {
        const int ch_ind = n.children[i];
        NarrowTreeNode & ch_n = n.tree->nodes_.ptrw()[ch_ind];
        const bool ch_intersects = node_intersects( ch_n, tree );
        if ( ch_intersects )
            return true;
    }
    return false;
}



PoolVector<Vector3> NarrowTree::lines()
{
    Vector<Vector3> ls;
    const int qty = nodes_.size();
    for ( int i=0; i<qty; i++ )
    {
        const NarrowTreeNode & n = nodes_.ptr()[i];
        const bool has_ch = n.hasChildren();
        if ( has_ch )
            continue;
        const bool occupied = (n.value > 0);
        if ( !occupied )
            continue;

        /*
        const Vector3 * vs = n.verts_;
        ls.push_back( vs[0] );
        ls.push_back( vs[1] );

        ls.push_back( vs[1] );
        ls.push_back( vs[2] );

        ls.push_back( vs[2] );
        ls.push_back( vs[3] );

        ls.push_back( vs[3] );
        ls.push_back( vs[0] );


        ls.push_back( vs[4] );
        ls.push_back( vs[5] );

        ls.push_back( vs[5] );
        ls.push_back( vs[6] );

        ls.push_back( vs[6] );
        ls.push_back( vs[7] );

        ls.push_back( vs[7] );
        ls.push_back( vs[4] );


        ls.push_back( vs[0] );
        ls.push_back( vs[4] );

        ls.push_back( vs[1] );
        ls.push_back( vs[5] );

        ls.push_back( vs[2] );
        ls.push_back( vs[6] );

        ls.push_back( vs[3] );
        ls.push_back( vs[7] );*/
    }

    PoolVector<Vector3> res;
    const int sz = ls.size();
    res.resize( sz );
    PoolVector<Vector3>::Write w = res.write();
    for ( int i=0; i<sz; i++ )
    {
        const Vector3 & v = ls.ptr()[i];
        w[i] = v;
    }

    return res;
}


bool NarrowTree::parent( const NarrowTreeNode & node, NarrowTreeNode * & parent )
{
    if ( node.parentAbsIndex < 0 )
    {
        parent = 0;
        return false;
    }

    parent = &( nodes_.ptrw()[ node.parentAbsIndex ] );
    return true;
}

int  NarrowTree::insert_node( NarrowTreeNode & node )
{
    nodes_.push_back( node );
    const int ind = static_cast<int>(nodes_.size()) - 1;
    NarrowTreeNode * nns = nodes_.ptrw();
    NarrowTreeNode & n = nns[ind];
    n.tree     = this;
    n.absIndex = ind;
    return ind;
}

void NarrowTree::update_node( const NarrowTreeNode & node )
{
    nodes_.ptrw()[ node.absIndex ] = node;
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




