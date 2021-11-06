
#include "octree_mesh_gd.h"

namespace Ign
{

static void find_mesh_instances( Node * node, const Transform & t, Vector<Transform> & transforms, Vector<MeshInstance *> & instances );



void OctreeMeshGd::_bind_methods()
{
    ClassDB::bind_method( D_METHOD("set_origin", "at"), &OctreeMeshGd::set_origin );
    ClassDB::bind_method( D_METHOD("get_origin"),       &OctreeMeshGd::get_origin, Variant::VECTOR3 );

    ClassDB::bind_method( D_METHOD("set_quat", "q"),    &OctreeMeshGd::set_quat );
    ClassDB::bind_method( D_METHOD("get_quat"),         &OctreeMeshGd::get_quat, Variant::QUAT );

    ClassDB::bind_method( D_METHOD("set_se3", "se3"),   &OctreeMeshGd::set_se3 );
    ClassDB::bind_method( D_METHOD("get_se3"),          &OctreeMeshGd::get_se3, Variant::OBJECT );

    ClassDB::bind_method( D_METHOD("rebuild"),           &OctreeMeshGd::rebuild );
    ClassDB::bind_method( D_METHOD("faces_qty"),         &OctreeMeshGd::faces_qty,  Variant::INT );
    ClassDB::bind_method( D_METHOD("face", "int"),       &OctreeMeshGd::face,       Variant::ARRAY );
	ClassDB::bind_method( D_METHOD("face_world", "int"), &OctreeMeshGd::face_world, Variant::ARRAY );

    ClassDB::bind_method( D_METHOD("intersects_ray",          "origin", "dir"), &OctreeMeshGd::intersects_ray,          Variant::BOOL );
    ClassDB::bind_method( D_METHOD("intersects_ray_face",     "origin", "dir"), &OctreeMeshGd::intersects_ray_face,     Variant::ARRAY );
    ClassDB::bind_method( D_METHOD("intersects_segment",      "start",  "end"), &OctreeMeshGd::intersects_segment,      Variant::BOOL );
    ClassDB::bind_method( D_METHOD("intersects_segment_face", "start",  "end"), &OctreeMeshGd::intersects_segment_face, Variant::ARRAY );

    ADD_PROPERTY( PropertyInfo( Variant::VECTOR3,   "r" ),   "set_origin", "get_origin" );
    ADD_PROPERTY( PropertyInfo( Variant::QUAT,      "q" ),   "set_quat",   "get_quat" );
    ADD_PROPERTY( PropertyInfo( Variant::OBJECT,    "se3" ), "set_se3",    "get_se3" );
}

OctreeMeshGd::OctreeMeshGd()
    : Node()
{
}

OctreeMeshGd::~OctreeMeshGd()
{
}

void OctreeMeshGd::set_origin( const Vector3 & at )
{
    _octree_mesh.set_origin( at );
}

const Vector3 & OctreeMeshGd::get_origin() const
{
    const Vector3 & at = _octree_mesh.get_origin();
    return at;
}

void OctreeMeshGd::set_quat( const Quat & q )
{
    _octree_mesh.set_quat( q );
}

const Quat & OctreeMeshGd::get_quat() const
{
    const Quat & q = _octree_mesh.get_quat();
    return q;
}

void OctreeMeshGd::set_se3( const Ref<Se3Ref> & rhs )
{
    const SE3 & se3 = rhs.ptr()->se3;
    const Vector3 r = se3.r();
    const Quat    q = se3.q();

    _octree_mesh.set_origin( r );
    _octree_mesh.set_quat( q );
}

Ref<Se3Ref> OctreeMeshGd::get_se3() const
{
    SE3 se3;
    const Vector3 r = _octree_mesh.get_origin();
    const Quat    q = _octree_mesh.get_quat();
    se3.set_r( r );
    se3.set_q( q );

    Ref<Se3Ref> ret;
    ret.instance();
    ret.ptr()->se3 = se3;
    return ret;
}

void OctreeMeshGd::rebuild()
{
    _octree_mesh.clear();

    Vector<MeshInstance *> meshes;
    Vector<Transform>      transforms;
    const Transform        root_t = Transform();

    find_mesh_instances( this, root_t, transforms, meshes );

    const int qty = meshes.size();
    for ( int i=0; i<qty; i++ )
    {
        const Transform t = transforms.ptr()[i];
        MeshInstance * mi = meshes.ptr()[i];
        Ref<Mesh> m = mi->get_mesh();

        _octree_mesh.append( t, m );
    }

    _octree_mesh.compute_face_properties();
}

int OctreeMeshGd::faces_qty()
{
    const int ret = _octree_mesh.faces_qty();
    return ret;
}

Array OctreeMeshGd::face( int ind )
{
    _ret_array.clear();
    const OctreeMesh::FaceProperties props = _octree_mesh.face_properties( ind );
    _ret_array.push_back( props.position );
    _ret_array.push_back( props.normal );
    _ret_array.push_back( props.area );
    return _ret_array;
}

Array OctreeMeshGd::face_world( int ind )
{
	_ret_array.clear();
	const OctreeMesh::FaceProperties props = _octree_mesh.face_properties_world( ind );
	_ret_array.push_back( props.position );
	_ret_array.push_back( props.normal );
	_ret_array.push_back( props.area );
	return _ret_array;
}


bool OctreeMeshGd::intersects_ray( const Vector3 & origin, const Vector3 & dir )
{
    const bool ret = _octree_mesh.intersects_ray( origin, dir );
    return ret;
}

Array OctreeMeshGd::intersects_ray_face( const Vector3 & origin, const Vector3 & dir )
{
    real_t dist;
    OctreeMesh::FaceProperties props;
    const bool ok = _octree_mesh.intersects_ray_face( origin, dir, dist, props );
    Array ret;
    ret.push_back( ok );
    if ( ok )
    {
        ret.push_back( dist );
        ret.push_back( props.position );
        ret.push_back( props.normal );
        ret.push_back( props.area );
    }
    return ret;
}

bool OctreeMeshGd::intersects_segment( const Vector3 & start, const Vector3 & end )
{
    const bool ret = _octree_mesh.intersects_segment( start, end );
    return ret;
}

Array OctreeMeshGd::intersects_segment_face( const Vector3 & start, const Vector3 & end )
{
    real_t dist;
    OctreeMesh::FaceProperties props;
    const bool ok = _octree_mesh.intersects_segment_face( start, end, dist, props );
    Array ret;
    ret.push_back( ok );
    if ( ok )
    {
        ret.push_back( dist );
        ret.push_back( props.position );
        ret.push_back( props.normal );
        ret.push_back( props.area );
    }
    return ret;
}


OctreeMesh * OctreeMeshGd::octree_mesh()
{
    return &_octree_mesh;
}

const OctreeMesh * OctreeMeshGd::octree_mesh() const
{
	return &_octree_mesh;
}







static void find_mesh_instances( Node * node, const Transform & t, Vector<Transform> & transforms, Vector<MeshInstance *> & instances )
{
	MeshInstance * mi = Node::cast_to<MeshInstance>( node );
	if (mi != nullptr)
	{
		const Transform mi_t = mi->get_transform();
		const Transform total_t = t * mi_t;
		transforms.push_back( total_t );
		instances.push_back( mi );
	}

	// Try to convert to spatial to retrieve the transform.
	Spatial * s = Node::cast_to<Spatial>( node );
	Transform new_t = t;
	if ( s != nullptr )
	{
		const Transform node_t = s->get_transform();
		new_t = new_t * node_t;
	}

	const int children_qty = node->get_child_count();
	for ( int i=0; i<children_qty; i++ )
	{
		Node * child_node = node->get_child( i );
		find_mesh_instances( child_node, new_t, transforms, instances );
	}
}



}





