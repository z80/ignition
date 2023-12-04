
#ifndef __VOL_GEOMETRY_H_
#define __VOL_GEOMETRY_H_

#include "data_types.h"
#include "cube_types.h"
#include "vol_data_types.h"

namespace Ign
{

class VolGeometry
{
public:
	VolGeometry();
	virtual ~VolGeometry();

	virtual Vector3d convert( const VolVectorInt & at, Float scale ) const;
};


}





#endif





