
#include "pbd_simulation_rid.h"
#include "Simulation.h"

using namespace PBD;

PbdSimulationRid::PbdSimulationRid()
{
	simulation = memnew( Simulation );
	SimulationModel * model = memnew( SimulationModel );
	simulation->setModel( model );
}

PbdSimulationRid::~PbdSimulationRid()
{
	SimulationModel * model = simulation->getModel();
	memfree( model );
	memfree( simulation );
}



