
#include "octree_mesh_gd.h"

static void find_mesh_instances( Node * node, const Transform & t, Vector<Transform> & transforms, Vector<MeshInstance *> & instances );



void OctreeMeshGd::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("rebuild"),     &OctreeMeshGd::rebuild );
	ClassDB::bind_method( D_METHOD("faces_qty"),   &OctreeMeshGd::faces_qty, Variant::INT );
	ClassDB::bind_method( D_METHOD("face", "int"), &OctreeMeshGd::face,      Variant::ARRAY );

	ClassDB::bind_method( D_METHOD("intersects_ray", "origin", "dir"), &OctreeMeshGd::intersects_ray, Variant::BOOL );
}

OctreeMeshGd::OctreeMeshGd()
	: Node()
{
}

OctreeMeshGd::~OctreeMeshGd()
{
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

bool OctreeMeshGd::intersects_ray( const Vector3 & origin, const Vector3 & dir )
{
	const bool ret = _octree_mesh.intersects_ray( origin, dir );
	return ret;
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







