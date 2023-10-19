
#ifndef __VOL_GEOMETRY_GD_H_
#define __VOL_GEOMETRY_GD_H_

#include "vol_geometry.h"
#include "vector3d.h"

namespace Ign
{

class VolGeometryGd: public VolGeometry
{
public:
	VolGeometryGd( const Vector3d & center );
	virtual ~VolGeometryGd();

	virtual Vector3d convert( const VolVectorInt & at, Float scale ) const override;

private:
	Vector3d _center;
};


}


#endif


