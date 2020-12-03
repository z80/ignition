

#ifndef __PBD_RIGID_BODY_RID_H_
#define __PBD_RIGID_BODY_RID_H_

#include "core/rid.h"

namespace PBD
{
	class RigidBody;
}

class PbdRigidBodyRid: public RID_Data
{
	RID self;

private:
	PBD::RigidBody * rigid_body;

public:
	_FORCE_INLINE_ void set_self(const RID & p_self)
	{
		self = p_self;
	}

	_FORCE_INLINE_ RID get_self() const
	{
		return self;
	}

	_FORCE_INLINE_ void set_rigid_body( PBD::RigidBody * rb )
	{
		rigid_body = rb;
	}

	_FORCE_INLINE_ PBD::RigidBody * get_rigid_body()
	{
		return rigid_body;
	}

	PbdRigidBodyRid();
	~PbdRigidBodyRid();
};




#endif








