
#include "octree_mesh_gd.h"

static void find_mesh_instances( Node * node, const Transform & t, Vector<Transform> & transforms, Vector<MeshInstance *> & instances );

void OctreeMeshGd::_bind_methods()
{
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
}

int OctreeMeshGd::faces_qty()
{
}

Array OctreeMeshGd::face( int ind )
{
}

bool OctreeMeshGd::intersects_ray( const Vector3 & origin, const Vector3 & dir )
{
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





