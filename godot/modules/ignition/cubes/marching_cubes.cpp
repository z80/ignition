
#include "marching_cubes.h"

namespace Ign
{


MarchingCubes::MarchingCubes()
{}

MarchingCubes::~MarchingCubes()
{}

Vector3d MarchingCubes::interpolate( const Vector3d & v0, const Vector3d & v1, const Float val0, const Float val1 ) const
{
	const Float d = val1 - val0;
	const Float ad = (d < 0.0) ? (-d) : d;
	Vector3d ret;
	if (ad < eps)
	{
		ret = (v0 + v1) * 0.5;
	}
	else
	{
		const Float mu = (iso_level - val0) / d;
		ret = v0 + (v1 - v0) * mu;
	}

	return ret;
}


}





