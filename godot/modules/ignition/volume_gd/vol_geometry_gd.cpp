
#include "vol_geometry_gd.h"

namespace Ign
{

VolGeometryGd::VolGeometryGd( const Vector3d & center )
	: VolGeometry(),
	  _center( center )
{
}

VolGeometryGd::~VolGeometryGd()
{
}

Vector3d VolGeometryGd::convert( const VolVectorInt & at, Float scale ) const
{
	const Vector3d relative( static_cast<Float>(at.x)*scale,
		                     static_cast<Float>(at.y)*scale,
		                     static_cast<Float>(at.z)*scale );
	const Vector3d absolute = relative + _center;

	return absolute;
}



}



