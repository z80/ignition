
#include "vol_geometry.h"

namespace Ign
{

VolGeometry::VolGeometry()
{
}

VolGeometry::~VolGeometry()
{
}

Vector3d VolGeometry::convert( const VolVectorInt & at, Float scale ) const
{
	const Vector3d ret( static_cast<Float>(at.x)*scale,
						static_cast<Float>(at.y)*scale,
						static_cast<Float>(at.z)*scale );
	return ret;
}








}

