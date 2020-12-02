
#ifndef __PBD_SERVER_H_
#define __PBD_SERVER_H_

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

class PbdServer : public Object
{
	GDCLASS(PbdServer, Object);

	static PbdServer * singleton;
	static void thread_func(void * p_udata);

public:
	PbdServer();
	~PbdServer();

	static PbdServer * get_singleton();
	Error init();

protected:
	static void _bind_methods();

private:
	// Resources it holds.
	RID_Owner<PbdSimulationRid>     simulation_owner;
	Set<RID>                        simulations;
	RID_Owner<PbdRigidBodyRid>      rigid_body_owner;
	Set<RID>                        rigid_bodies;
	RID_Owner<PbdJointHingeMposRid> joint_hinge_mpos_owner;
	Set<RID>                        joint_hinge_mpos;

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




