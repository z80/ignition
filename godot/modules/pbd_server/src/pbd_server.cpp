
#include "pbd_server.h"
#include "Simulation.h"

PbdServer * PbdServer::singleton = nullptr;

void PbdServer::thread_func( void * p_udata )
{
}

PbdServer::PbdServer()
	: Object()
{
	singleton = this;
}

PbdServer::~PbdServer()
{
	for ( Set<RID>::Element * e=joint_hinge_mpos.front(); e; e=e->next() )
		destroy_joint_hinge_mpos( e->get() );
	for ( Set<RID>::Element * e=rigid_bodies.front(); e; e=e->next() )
		destroy_rigid_body( e->get() );
	for ( Set<RID>::Element * e=simulations.front(); e; e=e->next() )
		destroy_simulation( e->get() );
}

PbdServer * PbdServer::get_singleton()
{
	return singleton;
}

Error PbdServer::init()
{
	return OK;
}

void PbdServer::_bind_methods()
{
}

RID PbdServer::create_simulation()
{
	PbdSimulationRid * ptr = memnew( PbdSimulationRid() );
	RID ret = simulation_owner.make_rid( ptr );
	ptr->set_self( ret );
	simulations.insert( ret );

	return ret;
}

bool PbdServer::destroy_simulation(RID rid)
{
	if ( simulation_owner.owns(rid) )
	{
		PbdSimulationRid * sim = simulation_owner.get(rid);
		simulation_owner.free( rid );
		simulations.erase( rid );
		memdelete( sim );

		return true;
	}

	return false;
}

RID PbdServer::create_rigid_body()
{
	PbdRigidBodyRid * ptr = memnew( PbdRigidBodyRid() );
	RID ret = rigid_body_owner.make_rid( ptr );
	ptr->set_self( ret );
	rigid_bodies.insert( ret );

	return ret;
}

bool PbdServer::destroy_rigid_body(RID rid)
{
	if ( simulation_owner.owns(rid) )
	{
		PbdRigidBodyRid * rb = rigid_body_owner.get(rid);
		simulation_owner.free( rid );
		rigid_bodies.erase( rid );
		memdelete( rb );

		return true;
	}

	return false;
}

RID PbdServer::create_joint_hinge_mpos()
{
	PbdJointHingeMposRid * ptr = memnew( PbdJointHingeMposRid() );
	RID ret = joint_hinge_mpos_owner.make_rid( ptr );
	ptr->set_self( ret );
	joint_hinge_mpos.insert( ret );

	return ret;
}

bool PbdServer::destroy_joint_hinge_mpos(RID rid)
{
	if ( joint_hinge_mpos_owner.owns(rid) )
	{
		PbdJointHingeMposRid * joint = joint_hinge_mpos_owner.get(rid);
		joint_hinge_mpos_owner.free( rid );
		joint_hinge_mpos.erase( rid );
		memdelete( joint );

		return true;
	}

	return false;
}











