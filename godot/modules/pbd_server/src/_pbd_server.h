
#ifndef __UNDERSCORE_PBD_SERVER_H_
#define __UNDERSCORE_PBD_SERVER_H_

#include "core/list.h"
#include "core/object.h"
#include "core/os/thread.h"
#include "core/os/mutex.h"
#include "core/rid.h"
#include "core/set.h"
#include "core/variant.h"

#include "pbd_simulation_rid.h"
#include "pbd_rigid_body_rid.h"
#include "pbd_joint_hinge_mpos_rid.h"

class _PbdServer: public Object
{
	GDCLASS(_PbdServer, Object);

	static _PbdServer * singleton;

public:
	_PbdServer();
	~_PbdServer();

	static _PbdServer * get_singleton();

protected:
	static void _bind_methods();

public:
	// Methods it exposes.
	RID  create_simulation();
	bool destroy_simulation(RID rid);

	RID  create_rigid_body();
	bool destroy_rigid_body(RID rid);

	RID  create_joint_hinge_mpos();
	bool destroy_joint_hinge_mpos(RID rid);
};



#endif




