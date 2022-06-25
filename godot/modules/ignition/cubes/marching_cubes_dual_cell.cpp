
#include "marching_cubes_dual_cell.h"
#include "marching_cubes_dual.h"

namespace Ign
{

MarchingCubesDualCell::MarchingCubesDualCell()
{
	self_index      = -1;
	index_in_parent = -1;
	parent_node     = nullptr;
	for ( int i=0; i<8; i++ )
		child_nodes[i] = nullptr;
	size   = -1;

	for ( int i=0; i<8; i++ )
		values[i] = 0.0;
}

MarchingCubesDualCell::~MarchingCubesDualCell()
{
}

MarchingCubesDualCell::MarchingCubesDualCell( const VectorInt & c0, const VectorInt & c1, const VectorInt & c2, const VectorInt & c3,
	                                          const VectorInt & c4, const VectorInt & c5, const VectorInt & c6, const VectorInt & c7 )
{
	corners[0] = c0;
	corners[1] = c1;
	corners[2] = c2;
	corners[3] = c3;

	corners[4] = c4;
	corners[5] = c5;
	corners[6] = c6;
	corners[7] = c7;
}

MarchingCubesDualCell::MarchingCubesDualCell( const MarchingCubesDualCell & inst )
{
	*this = inst;
}

const MarchingCubesDualCell & MarchingCubesDualCell::operator=( const MarchingCubesDualCell & inst )
{
	if ( this != &inst )
	{
		for ( int i=0; i<8; i++ )
		{
			corners[i]           = inst.corners[i];
			values[i]            = inst.values[i];
			vertices[i]          = inst.vertices[i];
			vertices_unscaled[i] = inst.vertices_unscaled[i];
		}
	}

	return *this;
}



}



