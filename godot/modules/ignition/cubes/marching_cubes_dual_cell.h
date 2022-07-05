
#ifndef __MARCHING_CUBES_DUAL_CELL_H_
#define __MARCHING_CUBES_DUAL_CELL_H_




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
class VolumeSource;
class DistanceScalerBase;

class MarchingCubesDualCell
{
public:
    MarchingCubesDualCell();
    ~MarchingCubesDualCell();
	MarchingCubesDualCell( const VectorInt & c0, const VectorInt & c1, const VectorInt & c2, const VectorInt & c3,
		                   const VectorInt & c4, const VectorInt & c5, const VectorInt & c6, const VectorInt & c7 );
	MarchingCubesDualCell( const MarchingCubesDualCell & inst );
    const MarchingCubesDualCell & operator=( const MarchingCubesDualCell & inst );


public:
    VectorInt vertices_int[8];
	Float     values[8];
	Vector3d  vertices_scaled[8];
	Vector3d  vertices[8];
};

}




#endif





