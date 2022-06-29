
#include "marching_cubes_dual_cell.h"
#include "marching_cubes_dual.h"

namespace Ign
{

MarchingCubesDualCell::MarchingCubesDualCell()
{
	for ( int i=0; i<8; i++ )
		values[i] = 0.0;
}

MarchingCubesDualCell::~MarchingCubesDualCell()
{
}

MarchingCubesDualCell::MarchingCubesDualCell( const VectorInt & c0, const VectorInt & c1, const VectorInt & c2, const VectorInt & c3,
	                                          const VectorInt & c4, const VectorInt & c5, const VectorInt & c6, const VectorInt & c7 )
{
	vertices_int[0] = c0;
	vertices_int[1] = c1;
	vertices_int[2] = c2;
	vertices_int[3] = c3;

	vertices_int[4] = c4;
	vertices_int[5] = c5;
	vertices_int[6] = c6;
	vertices_int[7] = c7;
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
			vertices_int[i]      = inst.vertices_int[i];
			values[i]            = inst.values[i];
			vertices[i]          = inst.vertices[i];
			vertices_unscaled[i] = inst.vertices_unscaled[i];
		}
	}

	return *this;
}



}



