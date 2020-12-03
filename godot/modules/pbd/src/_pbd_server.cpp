
#include "_pbd_server.h"
#include "pbd_server.h"

_PbdServer * _PbdServer::singleton = nullptr;

_PbdServer::_PbdServer()
	: Object()
{
	singleton = this;
}

_PbdServer::~_PbdServer()
{
}

_PbdServer * _PbdServer::get_singleton()
{
	return singleton;
}


void _PbdServer::_bind_methods()
{
	// Don't expose methods to the script.
	// At least not yet.
}

RID _PbdServer::create_simulation()
{
	return PbdServer::get_singleton()->create_simulation();
}

bool _PbdServer::destroy_simulation(RID rid)
{
	return PbdServer::get_singleton()->destroy_simulation( rid );
}

RID _PbdServer::create_rigid_body()
{
	return PbdServer::get_singleton()->create_rigid_body();
}

bool _PbdServer::destroy_rigid_body(RID rid)
{
	return PbdServer::get_singleton()->destroy_rigid_body( rid );
}

RID _PbdServer::create_joint_hinge_mpos()
{
	return PbdServer::get_singleton()->create_joint_hinge_mpos();
}

bool _PbdServer::destroy_joint_hinge_mpos(RID rid)
{
	return PbdServer::get_singleton()->destroy_joint_hinge_mpos( rid );
}











