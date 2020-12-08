
#include "pbd_rigid_body_rid.h"
#include "RigidBody.h"
#include "CollisionDetection.h"

using namespace PBD;

class PbdRigidBodyRid::PD
{
public:
	PBD::RigidBody * rigid_body;
	PBD::CollisionDetection::CollisionObject * collision_object;

	PD()
	{
		rigid_body = nullptr;
		collision_object = nullptr;
	}

	~PD()
	{
		if ( rigid_body != nullptr )
		{
			memfree( rigid_body );
			rigid_body = nullptr;
		}

		if ( collision_object != nullptr )
		{
			memfree( collision_object );
			collision_object = nullptr;
		}
	}
};

PbdRigidBodyRid::PbdRigidBodyRid()
{
	pd = memnew( PD() );
	pd->rigid_body = memnew( RigidBody() );
}

PbdRigidBodyRid::~PbdRigidBodyRid()
{
	memfree( pd );
	pd = nullptr;
}

void PbdRigidBodyRid::set_rigid_body( PBD::RigidBody * rb )
{
	if ( pd->rigid_body != nullptr )
		memfree( pd->rigid_body );
	pd->rigid_body = rb;
}

PBD::RigidBody * PbdRigidBodyRid::get_rigid_body()
{
	return pd->rigid_body;
}

void PbdRigidBodyRid::set_collision_object( void * p )
{
	PBD::CollisionDetection::CollisionObject * co = reinterpret_cast<PBD::CollisionDetection::CollisionObject *>( p );
	if ( pd->collision_object != nullptr )
		memfree( pd->collision_object );
	pd->collision_object = co;
}

void * PbdRigidBodyRid::get_collision_object()
{
	return reinterpret_cast<void *>( pd->collision_object );
}



