
#ifndef __VOL_SOURCE_GD_H_
#define __VOL_SOURCE_GD_H_

#include "vol_source.h"

#include "servers/physics_server_3d.h"

namespace Ign
{

class VolSourceGd: public VolSource
{
public:
	VolSourceGd( PhysicsDirectSpaceState3D * state );
	virtual ~VolSourceGd();

	virtual bool intersects( const Vector3d & start, const Vector3d & end, Float & v_start, Float & v_end, Vector3d & at ) const override;

private:
	PhysicsDirectSpaceState3D * _state;
};


}




#endif

