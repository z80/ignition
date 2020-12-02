
#include "pbd_rigid_body_rid.h"
#include "RigidBody.h"

using namespace PBD;

PbdRigidBodyRid::PbdRigidBodyRid()
{
	rigid_body = memnew( RigidBody() );
}

PbdRigidBodyRid::~PbdRigidBodyRid()
{
	memfree( rigid_body );
}



