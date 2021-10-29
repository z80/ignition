
#include "scene/main/node.h"
#include "scene/3d/mesh_instance.h"

#include "octree_mesh.h"

class OctreeMeshGd: public Node
{
	GDCLASS(OctreeMeshGd, Node);
	OBJ_CATEGORY("Ignition");

protected:
	static void _bind_methods();

public:
	OctreeMeshGd();
	~OctreeMeshGd();

	void rebuild();
	int faces_qty();
	Array face( int ind );
	bool intersects_ray( const Vector3 & origin, const Vector3 & dir );

private:
	OctreeMesh _octree_mesh;
	Array     _ret_array;
};



