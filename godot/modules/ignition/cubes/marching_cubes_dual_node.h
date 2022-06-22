
#include "data_types.h"
#include "vector3d.h"
#include "core/vector.h"
#include "core/math/face3.h"

#include "se3.h"


#include <vector>
#include <set>
#include <map>

#include "cube_types.h"


namespace Ign
{

class MarchingCubesDual;

class MarchingCubesDualNode
{
public:
	MarchingCubesDualNode();
	~MarchingCubesDualNode();
	MarchingCubesDualNode( const MarchingCubesDualNode & inst );
	const MarchingCubesDualNode & operator=( const MarchingCubesDualNode & inst );

	bool has_children() const;
	bool subdivide( MarchingCubesDual * tree );

	// Initialize vertices and planes.
	void init( MarchingCubesDual * tree );

	const VectorInt center() const;

	bool has_surface() const;

public:
	int self_index;
	int index_in_parent;

	// Parent node.
	MarchingCubesDualNode * parent_node;
	// Child indices in Octtree list.
	MarchingCubesDualNode * child_nodes[8];

	int       size; // Size.
	VectorInt at;

	// Store these for convenience.
	VectorInt vertices_int[8];
	Vector3d  vertices[8];
	Vector3d  vertices_unscaled[8];
	Float     values[8];
};

}





