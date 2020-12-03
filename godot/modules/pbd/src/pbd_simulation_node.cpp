
#include "pbd_simulation_node.h"
#include "pbd_server.h"

PbdSimulationNode::PbdSimulationNode()
	: Spatial()
{
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
}



void PbdSimulationNode::step()
{
}

bool PbdSimulationNode::init()
{
	//sim_rid.

	return false;
}

void PbdSimulationNode::finit()
{
	//if ( sim_rid.self_id() != 0 )
	{

	}
}






