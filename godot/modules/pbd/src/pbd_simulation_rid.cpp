
#include "pbd_simulation_rid.h"
#include "Simulation.h"
#include "TimeManager.h"
#include "DistanceFieldCollisionDetection.h"

using namespace PBD;

PbdSimulationRid::PbdSimulationRid()
{
	simulation = memnew( Simulation );
	SimulationModel * model = memnew( SimulationModel );
	simulation->setModel( model );
	simulation->init();

	time_manager = memnew( TimeManager );
	collision_detection = memnew( DistanceFieldCollisionDetection );
}


PbdSimulationRid::~PbdSimulationRid()
{
	SimulationModel * model = simulation->getModel();
	memfree( model );
	memfree( simulation );

	memfree( time_manager );

	// Don't allow "collision_detection" to delete objects.
	// Those are managed in rigid bodies.
	std::vector<PBD::CollisionDetection::CollisionObject*> & collision_objects = collision_detection->getCollisionObjects();
	collision_objects.clear();
	memfree( collision_detection );
}



