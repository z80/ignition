
#ifndef __VOL_MC_GD_H_
#define __VOL_MC_GD_H_

#include "core/object/ref_counted.h"
#include "servers/physics_server_3d.h"

namespace Ign
{

class VolMcGd: public RefCounted
{
	GDCLASS(VolMcGd, RefCounted);
protected:
	static void _bind_methods();

public:
	VolMcGd();
	virtual ~VolMcGd();

	Array build_surface( const Vector3 & center, Object * physics_direct_space_state_3d, real_t scale=0.1, int subdivisions=2 );
};



}





#endif





