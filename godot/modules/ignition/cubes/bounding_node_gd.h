
#include "core/reference.h"
#include "se3_ref.h"
#include "marching_cubes_dual_node.h"



#ifndef __BOUNDING_NODE_H_
#define __BOUNDING_NODE_H_

namespace Ign
{

class BoundingNodeGd: public Reference
{
	GDCLASS(BoundingNodeGd, Reference);
protected:
	static void _bind_methods();

public:
	BoundingNodeGd();
	~BoundingNodeGd();

	Ref<BoundingNodeGd> create_adjacent_node( int dx, int dy, int dz ) const;

public:
	MarchingCubesDualNode node;
};


}


#endif

