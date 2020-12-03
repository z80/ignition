
#include "pbd_simulation_node.h"
#include "pbd_server.h"
#include "pbd_simulation_rid.h"

#include "Simulation.h"
#include "TimeStep.h"

PbdSimulationNode::PbdSimulationNode()
	: Spatial()
{
	sim = nullptr;
}

PbdSimulationNode::~PbdSimulationNode()
{
	finit();
}

void PbdSimulationNode::_notification( int p_what )
{
	switch ( p_what )
	{
		case NOTIFICATION_ENTER_TREE:
			ERR_FAIL_COND( init() );
			break;

		case NOTIFICATION_EXIT_TREE:
			finit();
			break;
	}
}

void PbdSimulationNode::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("step"), &PbdSimulationNode::step );
}



void PbdSimulationNode::step()
{
	if ( sim == nullptr )
		return;

	PBD::Simulation * s = sim->get_simulation();
	PBD::SimulationModel * sm = s->getModel();
	PBD::TimeStep * ts = s->getTimeStep();
	ts->step( *sm );
}

bool PbdSimulationNode::init()
{
	const bool valid_ok = sim_rid.is_valid();
	if ( valid_ok )
		return true;

	sim_rid = PbdServer::get_singleton()->create_simulation();
	const bool new_is_valid = sim_rid.is_valid();
	if ( new_is_valid )
		sim = PbdServer::get_singleton()->get_simulation( sim_rid );
	else
		sim = nullptr;

	return new_is_valid;
}

void PbdSimulationNode::finit()
{
	const bool valid_ok = sim_rid.is_valid();
	if ( !valid_ok )
		return;

	PbdServer::get_singleton()->destroy_simulation( sim_rid );
	sim_rid = RID();
	sim = nullptr;
}






