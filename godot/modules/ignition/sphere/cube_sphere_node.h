
#ifndef __CUBE_SHPERE_NODE_H_
#define __CUBE_SPHERE_NODE_H_

#include "scene/3d/mesh_instance.h"
#include "scene/resources/mesh.h"

#include "cube_sphere.h"
#include "subdivide_source.h"
#include "height_source_test.h"

namespace Ign
{

class CubeSphereNode: public MeshInstance
{
	GDCLASS( CubeSphereNode, MeshInstance );
public:
	CubeSphereNode();
	~CubeSphereNode();


protected:
	static void _bind_methods();

public:
	// This one is for constructing actual mesh.
	ArrayMesh      * array_mesh;

	// These all are for geometry generation.
	CubeSphere       sphere;
	SubdivideSource  subdivide_source;
	HeightSourceTest height_source;
};


}





#endif



