
#include "octree_mesh_gd.h"

namespace Ign
{

static void find_mesh_instances( Node * node, const Transform3D & t, Vector<Transform3D> & transforms, Vector<MeshInstance3D *> & instances );



void OctreeMeshGd::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("set_max_depth", "val"), &OctreeMeshGd::set_max_depth );
	ClassDB::bind_method( D_METHOD("get_max_depth"),        &OctreeMeshGd::get_max_depth );
	ClassDB::bind_method( D_METHOD("set_min_faces", "val"), &OctreeMeshGd::set_min_faces );
	ClassDB::bind_method( D_METHOD("get_min_faces"),        &OctreeMeshGd::get_min_faces );

    ClassDB::bind_method( D_METHOD("set_origin", "at"), &OctreeMeshGd::set_origin );
    ClassDB::bind_method( D_METHOD("get_origin"),       &OctreeMeshGd::get_origin );

    ClassDB::bind_method( D_METHOD("set_quat", "q"),    &OctreeMeshGd::set_quat );
    ClassDB::bind_method( D_METHOD("get_quat"),         &OctreeMeshGd::get_quat );

    ClassDB::bind_method( D_METHOD("set_se3", "se3"),   &OctreeMeshGd::set_se3 );
    ClassDB::bind_method( D_METHOD("get_se3"),          &OctreeMeshGd::get_se3 );

    ClassDB::bind_method( D_METHOD("rebuild"),               &OctreeMeshGd::rebuild );
    ClassDB::bind_method( D_METHOD("faces_qty"),             &OctreeMeshGd::faces_qty );
    ClassDB::bind_method( D_METHOD("get_face", "int"),       &OctreeMeshGd::get_face );
	ClassDB::bind_method( D_METHOD("get_face_world", "int"), &OctreeMeshGd::get_face_world );

    ClassDB::bind_method( D_METHOD("intersects_ray",          "origin", "dir"), &OctreeMeshGd::intersects_ray );
    ClassDB::bind_method( D_METHOD("intersects_ray_face",     "origin", "dir"), &OctreeMeshGd::intersects_ray_face );
    ClassDB::bind_method( D_METHOD("intersects_segment",      "start",  "end"), &OctreeMeshGd::intersects_segment );
    ClassDB::bind_method( D_METHOD("intersects_segment_face", "start",  "end"), &OctreeMeshGd::intersects_segment_face );

	ADD_PROPERTY( PropertyInfo( Variant::INT, "max_depth" ), "set_max_depth", "get_max_depth" );
	ADD_PROPERTY( PropertyInfo( Variant::INT, "min_faces" ), "set_min_faces", "get_min_faces" );

    ADD_PROPERTY( PropertyInfo( Variant::VECTOR3,    "r" ),   "set_origin", "get_origin" );
    ADD_PROPERTY( PropertyInfo( Variant::QUATERNION, "q" ),   "set_quat",   "get_quat" );
    ADD_PROPERTY( PropertyInfo( Variant::OBJECT,     "se3" ), "set_se3",    "get_se3" );
}

void OctreeMeshGd::_notification( int p_notification )
{
	switch (p_notification)
	{
	case NOTIFICATION_READY:
		rebuild();
		break;
	}

}

OctreeMeshGd::OctreeMeshGd()
    : Node()
{
}

OctreeMeshGd::~OctreeMeshGd()
{
}

void OctreeMeshGd::set_max_depth( int val )
{
	_octree_mesh.set_max_depth( val );
}

int OctreeMeshGd::get_max_depth() const
{
	const int ret = _octree_mesh.get_max_depth();
	return ret;
}

void OctreeMeshGd::set_min_faces( int val )
{
	_octree_mesh.set_min_faces( val );
}

int OctreeMeshGd::get_min_faces() const
{
	const int ret = _octree_mesh.get_min_faces();
	return ret;
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

void OctreeMeshGd::set_quat( const Quaternion & q )
{
    _octree_mesh.set_quat( q );
}

const Quaternion & OctreeMeshGd::get_quat() const
{
    const Quaternion & q = _octree_mesh.get_quat();
    return q;
}

void OctreeMeshGd::set_se3( const Ref<Se3Ref> & rhs )
{
    const SE3 & se3 = rhs.ptr()->se3;

    const Vector3    r = se3.r();
    const Quaternion q = se3.q();

    _octree_mesh.set_origin( r );
    _octree_mesh.set_quat( q );
}

Ref<Se3Ref> OctreeMeshGd::get_se3() const
{
    SE3 se3;
    const Vector3    r = _octree_mesh.get_origin();
    const Quaternion q = _octree_mesh.get_quat();
    se3.set_r( r );
    se3.set_q( q );

    Ref<Se3Ref> ret;
    ret.instantiate();
    ret.ptr()->se3 = se3;
    return ret;
}

void OctreeMeshGd::rebuild()
{
    _octree_mesh.clear();

    Vector<MeshInstance3D *> meshes;
    Vector<Transform3D>      transforms;
    const Transform3D        root_t = Transform3D();

    find_mesh_instances( this, root_t, transforms, meshes );

    const int qty = meshes.size();
    for ( int i=0; i<qty; i++ )
    {
        const Transform3D t = transforms.ptr()[i];
        MeshInstance3D * mi = meshes.ptr()[i];
        Ref<Mesh> m = mi->get_mesh();

        _octree_mesh.append( t, m );
    }

	_octree_mesh.subdivide();
}

int OctreeMeshGd::faces_qty()
{
    const int ret = _octree_mesh.faces_qty();
    return ret;
}

Array OctreeMeshGd::get_face( int ind )
{
    _ret_array.clear();
    const OctreeMesh::FaceProperties props = _octree_mesh.face_properties( ind );
    _ret_array.push_back( props.position );
    _ret_array.push_back( props.normal );
    _ret_array.push_back( props.area );
    return _ret_array;
}

Array OctreeMeshGd::get_face_world( int ind )
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
	Vector3 at;
    OctreeMesh::FaceProperties props;
    const bool ok = _octree_mesh.intersects_segment_face( start, end, dist, at, props );
    Array ret;
    ret.push_back( ok );
    if ( ok )
    {
        ret.push_back( dist );
		ret.push_back( at );
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







static void find_mesh_instances( Node * node, const Transform3D & t, Vector<Transform3D> & transforms, Vector<MeshInstance3D *> & instances )
{
	MeshInstance3D * mi = Node::cast_to<MeshInstance3D>( node );
	if (mi != nullptr)
	{
		const Transform3D mi_t = mi->get_transform();
		const Transform3D total_t = t * mi_t;
		transforms.push_back( total_t );
		instances.push_back( mi );
	}

	// Try to convert to spatial to retrieve the transform.
	Node3D * s = Node::cast_to<Node3D>( node );
	Transform3D new_t = t;
	if ( s != nullptr )
	{
		const Transform3D node_t = s->get_transform();
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





