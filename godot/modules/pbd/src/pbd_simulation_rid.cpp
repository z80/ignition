
#include "pbd_simulation_rid.h"
#include "Simulation.h"
#include "TimeManager.h"

using namespace PBD;

PbdSimulationRid::PbdSimulationRid()
{
	simulation = memnew( Simulation );
	SimulationModel * model = memnew( SimulationModel );
	simulation->setModel( model );
	simulation->init();

	time_manager = memnew( TimeManager );
}

PbdSimulationRid::~PbdSimulationRid()
{
	SimulationModel * model = simulation->getModel();
	memfree( model );
	memfree( simulation );

	memfree( time_manager );
}



