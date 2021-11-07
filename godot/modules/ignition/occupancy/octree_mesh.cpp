
#include "octree_mesh.h"

namespace Ign
{

static void faces_from_surface( const Transform & t, const Mesh & mesh, int surface_idx, Vector<Face3> & faces );
static void parse_mesh_arrays( const Transform & t, const Mesh & mesh, int surface_idx, bool is_index_array, Vector<Face3> & faces );


OctreeMesh::OctreeMesh()
{
    // Initialize counters and parameters.
    this->max_depth_ = 5;
    this->min_faces_ = 5;
}

OctreeMesh::~OctreeMesh()
{
}

void OctreeMesh::set_max_depth( int val )
{
	max_depth_ = val;
}

int OctreeMesh::get_max_depth() const
{
	return max_depth_;
}

void OctreeMesh::set_min_faces( int val )
{
	min_faces_ = val;
}

int OctreeMesh::get_min_faces() const
{
	return min_faces_;
}

void OctreeMesh::clear()
{
    nodes_.clear();
    faces_.clear();
}

void OctreeMesh::append( const Transform & t, const Ref<Mesh> mesh )
{
    const int qty = mesh->get_surface_count();
    for ( int i=0; i<qty; i++ )
    {
        const Mesh & m = **mesh;
        faces_from_surface( t, m, i, faces_ );
    }
}

void OctreeMesh::subdivide()
{
    // Compute face properties for future use.
    compute_face_properties();

    // Get average of all the face points.
    const int qty = faces_.size();
    Vector3 c;
    c.zero();
    for ( int i=0; i<qty; i++ )
    {
        const Face3 & f = faces_.ptr()[i];
        c += f.vertex[0];
        c += f.vertex[1];
        c += f.vertex[2];
    }
    const real_t s = 3.0 * static_cast<real_t>( qty );
    c /= s;

    // Here find the largest distance.
    real_t d = 0.0;
    for ( int i=0; i<qty; i++ )
    {
        const Face3 & f = faces_.ptr()[i];
        for ( int j=0; j<3; j++ )
        {
            const Vector3 dv = f.vertex[j] - c;
            real_t l = dv.length();
            if ( l > d )
                d = l;
        }
    }

	// To avoid numerical incertainty.
	d *= 1.1;

    nodes_.clear();
    OctreeMeshNode root;
    root.level = 0;
    root.tree = this;
    root.center = c;
    root.size2  = d;
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
}

void OctreeMesh::set_origin( const Vector3 & at )
{
    origin_ = at;
}

const Vector3 & OctreeMesh::get_origin() const
{
    return origin_;
}

void OctreeMesh::set_quat( const Quat & q )
{
    quat_ = q;
}

const Quat & OctreeMesh::get_quat() const
{
    return quat_;
}

void OctreeMesh::set_se3( const SE3 & se3 )
{
    const Vector3 r = se3.r();
    const Quat    q = se3.q();

    origin_ = r;
    quat_   = q;
}

SE3  OctreeMesh::get_se3() const
{
    SE3 se3;
    se3.set_r( origin_ );
    se3.set_q( quat_ );

    return se3;
}

real_t OctreeMesh::size2() const
{
	if ( nodes_.empty() )
		return 0.0;
    const OctreeMeshNode & node = nodes_.ptr()[0];
    return node.size2;
}

bool OctreeMesh::intersects_ray( const Vector3 & origin, const Vector3 & dir ) const
{
    const Quat    inv_quat     = quat_.inverse();
    const Vector3 origin_local = inv_quat.xform( origin - origin_ );
    const Vector3 dir_local    = inv_quat.xform( dir );
    const OctreeMeshNode & root = nodes_.ptr()[0];
    const bool res = root.intersects_ray( origin_local, dir_local );
    return res;
}

bool OctreeMesh::intersects_ray_face( const Vector3 & origin, const Vector3 & dir, real_t & face_dist, FaceProperties & fp ) const
{
    const Quat    inv_quat     = quat_.inverse();
    const Vector3 origin_local = inv_quat.xform( origin - origin_ );
    const Vector3 dir_local    = inv_quat.xform( dir );
    const OctreeMeshNode & root = nodes_.ptr()[0];
    int face_ind = -1;
    real_t dist = 0.0;
    const bool res = root.intersects_ray_face( origin_local, dir_local, face_ind, dist );
    if ( res )
    {
        face_dist = dist;

        fp = face_props_.ptr()[face_ind];
        fp.position = quat_.xform( fp.position ) + origin_;

        fp.normal = quat_.xform( fp.normal );
    }
    return res;
}

bool OctreeMesh::intersects_segment( const Vector3 & start, const Vector3 & end ) const
{
    const Quat    inv_quat     = quat_.inverse();
    const Vector3 start_local = inv_quat.xform( start - origin_ );
    const Vector3 end_local   = inv_quat.xform( end - origin_ );
    const OctreeMeshNode & root = nodes_.ptr()[0];
    const bool res = root.intersects_ray( start_local, end_local );
    return res;
}

bool OctreeMesh::intersects_segment_face( const Vector3 & start, const Vector3 & end, real_t & face_dist, Vector3 & face_at, FaceProperties & fp ) const
{
    const Quat    inv_quat     = quat_.inverse();
    const Vector3 origin_local = inv_quat.xform( start - origin_ );
    const Vector3 dir_local    = inv_quat.xform( end - origin_ );
    const OctreeMeshNode & root = nodes_.ptr()[0];
    int face_ind = -1;
    real_t dist = 0.0;
	Vector3 at;
    const bool res = root.intersects_segment_face( origin_local, dir_local, face_ind, dist, at );
    if ( res )
    {
        face_dist = dist;

		face_at = quat_.xform( at ) + origin_;

        fp = face_props_.ptr()[face_ind];
        fp.position = quat_.xform( fp.position ) + origin_;

        fp.normal = quat_.xform( fp.normal );
    }
    return res;
}

PoolVector<Vector3> OctreeMesh::lines()
{
    Vector<Vector3> ls;
    const int qty = nodes_.size();
    for ( int i=0; i<qty; i++ )
    {
        const OctreeMeshNode & n = nodes_.ptr()[i];
        const bool has_ch = n.hasChildren();
        if ( has_ch )
                continue;
        const bool is_empty = n.ptInds.empty();
        if ( is_empty )
                continue;

        //const Vector3 * vs = n.verts_;
        //ls.push_back( vs[0] );
        //ls.push_back( vs[1] );

        //ls.push_back( vs[1] );
        //ls.push_back( vs[2] );

        //ls.push_back( vs[2] );
        //ls.push_back( vs[3] );

        //ls.push_back( vs[3] );
        //ls.push_back( vs[0] );


        //ls.push_back( vs[4] );
        //ls.push_back( vs[5] );

        //ls.push_back( vs[5] );
        //ls.push_back( vs[6] );

        //ls.push_back( vs[6] );
        //ls.push_back( vs[7] );

        //ls.push_back( vs[7] );
        //ls.push_back( vs[4] );


        //ls.push_back( vs[0] );
        //ls.push_back( vs[4] );

        //ls.push_back( vs[1] );
        //ls.push_back( vs[5] );

        //ls.push_back( vs[2] );
        //ls.push_back( vs[6] );

        //ls.push_back( vs[3] );
        //ls.push_back( vs[7] );
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


bool OctreeMesh::parent( const OctreeMeshNode & node, OctreeMeshNode * & parent )
{
    if ( node.parentAbsIndex < 0 )
    {
        parent = nullptr;
        return false;
    }

    parent = &( nodes_.ptrw()[ node.parentAbsIndex ] );
    return true;
}

int  OctreeMesh::insert_node( OctreeMeshNode & node )
{
    nodes_.push_back( node );
    const int ind = static_cast<int>(nodes_.size()) - 1;
    OctreeMeshNode * nns = nodes_.ptrw();
    OctreeMeshNode & n   = nns[ind];
    n.tree     = this;
    n.absIndex = ind;
    return ind;
}

void OctreeMesh::update_node( const OctreeMeshNode & node )
{
    nodes_.ptrw()[ node.absIndex ] = node;
}

void OctreeMesh::compute_face_properties()
{
    face_props_.clear();
    const int qty = faces_.size();
    for ( int i=0; i<qty; i++ )
    {
        const Face3 & f   = faces_.ptr()[i];
        const real_t area = f.get_area();
        const Plane p     = f.get_plane();
        const Vector3 n   = p.get_normal();
        const Vector3 r0  = ( f.vertex[0] + f.vertex[1] + f.vertex[2] ) / 3.0;
        FaceProperties props;
        props.area     = area;
        props.normal   = n;
        props.position = r0;
        face_props_.push_back( props );
    }
}

int OctreeMesh::faces_qty() const
{
    const int ret = faces_.size();
    return ret;
}

OctreeMesh::FaceProperties OctreeMesh::face_properties( int ind ) const
{
    const FaceProperties & ret = face_props_.ptr()[ind];
    return ret;
}

OctreeMesh::FaceProperties OctreeMesh::face_properties_world( int ind ) const
{
	FaceProperties ret = face_props_.ptr()[ind];
	ret.normal = quat_.xform( ret.normal );
	ret.position = quat_.xform( ret.position ) + origin_;
	return ret;
}

















static void faces_from_surface( const Transform & t, const Mesh & mesh, int surface_idx, Vector<Face3> & faces )
{
	// Don't add faces if doesn't consist of triangles.
    if (mesh.surface_get_primitive_type(surface_idx) != Mesh::PRIMITIVE_TRIANGLES)
        return;

    const uint32_t type = mesh.surface_get_format( surface_idx );
    const bool is_index_array = (type & Mesh::ARRAY_FORMAT_INDEX);
    parse_mesh_arrays( t, mesh, surface_idx, is_index_array, faces );
}

static void parse_mesh_arrays( const Transform & t, const Mesh & mesh, int surface_idx, bool is_index_array, Vector<Face3> & faces )
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
            Face3 & face = faces.ptrw()[ orig_qty + face_idx ];
            const Vector3 v = vertices_reader[lookup_index];
            const Vector3 vt = t.xform( v );
            face.vertex[set_offset] = vt;
        }
    }
    else
    {
        for (int i = 0; i < vert_count; i++)
        {
            const int face_idx   = i / 3;
            const int set_offset = i % 3;
            const int lookup_index = i;
            Face3 & face = faces.ptrw()[ orig_qty + face_idx ];
            const Vector3 v = vertices_reader[lookup_index];
            const Vector3 vt = t.xform( v );
            face.vertex[set_offset] = vt;
        }
    }
}


}


