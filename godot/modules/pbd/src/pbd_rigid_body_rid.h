

#ifndef __PBD_RIGID_BODY_RID_H_
#define __PBD_RIGID_BODY_RID_H_

#include "core/rid.h"

namespace PBD
{
	class RigidBody;
}

class PbdRigidBodyRid: public RID_Data
{
private:
	RID self;

	//PBD::RigidBody * rigid_body;
	//PBD::CollisionObject * collision_object;
	class PD;
	PD * pd;

public:
	_FORCE_INLINE_ void set_self(const RID & p_self)
	{
		self = p_self;
	}

	_FORCE_INLINE_ RID get_self() const
	{
		return self;
	}

	void set_rigid_body( PBD::RigidBody * rb );
	PBD::RigidBody * get_rigid_body();
	void set_collision_object( void * p );
	void * get_collision_object();

	PbdRigidBodyRid();
	~PbdRigidBodyRid();
};




#endif








