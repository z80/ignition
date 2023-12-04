
#ifndef __VOL_SOURCE_H_
#define __VOL_SOURCE_H_

#include "data_types.h"
#include "vector3d.h"

namespace Ign
{


class VolSource
{
public:
	VolSource();
	virtual ~VolSource();

	virtual bool intersects( const Vector3d & start, const Vector3d & end, Float & v_start, Float & v_end, Vector3d & at ) const;
};


}


#endif

