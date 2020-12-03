


#ifndef __PBD_JOINT_HINGE_MPOS_RID_H_
#define __PBD_JOINT_HINGE_MPOS_RID_H_

#include "core/rid.h"

namespace PBD
{
	class Constraint;
}

class PbdJointHingeMposRid: public RID_Data
{
	RID self;

private:
	PBD::Constraint * constraint;

public:
	_FORCE_INLINE_ void set_self( const RID & p_self )
	{
		self = p_self;
	}

	_FORCE_INLINE_ RID get_self() const
	{
		return self;
	}

	_FORCE_INLINE_ void set_constraint( PBD::Constraint * c )
	{
		constraint = c;
	}

	_FORCE_INLINE_ PBD::Constraint * get_constraint()
	{
		return constraint;
	}

	PbdJointHingeMposRid();
	~PbdJointHingeMposRid();
};




#endif











