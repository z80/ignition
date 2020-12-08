
#include "pbd_simulation_node.h"
#include "pbd_server.h"
#include "pbd_simulation_rid.h"

#include "Simulation.h"
#include "TimeStep.h"
#include "TimeManager.h"
#include "DistanceFieldCollisionDetection.h"

#include "pbd_rigid_body_node.h"

PbdSimulationNode::PbdSimulationNode()
	: Spatial()
{
	sim = nullptr;
	t = 0.0;
}

PbdSimulationNode::~PbdSimulationNode()
{
	finit();
}

void PbdSimulationNode::_notification( int p_what )
{
	Spatial::_notification( p_what );

	switch ( p_what )
	{
		case NOTIFICATION_ENTER_TREE:
			//ERR_FAIL_COND( init() );
			init();
			break;

		// Destroy when it is physically deleted.
		// Have to destroy bodies and joints before simulation. But don't know how
		// based on the fact that there is no pre-exit event.
		//case NOTIFICATION_EXIT_TREE:
		//	finit();
		//	break;
		default:
			break;
	}
}

void PbdSimulationNode::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("start"),              &PbdSimulationNode::start );
	ClassDB::bind_method( D_METHOD("rescan"),             &PbdSimulationNode::rescan );
	ClassDB::bind_method( D_METHOD("set_time_step", "h"), &PbdSimulationNode::set_time_step );
	ClassDB::bind_method( D_METHOD("step", "delta"),      &PbdSimulationNode::step );
}



void PbdSimulationNode::step( real_t dt )
{
	if ( sim == nullptr )
		return;

	PBD::Simulation * s = sim->get_simulation();
	PBD::Simulation::setCurrent( s );

	PBD::SimulationModel * sm = s->getModel();
	// Setup time step size.
	PBD::TimeManager * tm = sim->get_time_manager();
	PBD::TimeManager::setCurrent( tm );

	PBD::TimeStep * ts = s->getTimeStep();
	PBD::DistanceFieldCollisionDetection * cfcd = sim->get_collision_detection();
	ts->setCollisionDetection( *sm, cfcd );


	const Real h = tm->getTimeStepSize();
	t += dt;
	while ( t >= h )
	{
		ts->step( *sm );
		t -= h;
	}
}

void PbdSimulationNode::start()
{
	rescan();
}

void PbdSimulationNode::rescan()
{
	if ( !sim )
		return;
	PBD::Simulation * s = sim->get_simulation();
	PBD::SimulationModel * sm = s->getModel();
	PBD::SimulationModel::RigidBodyVector & rbs = sm->getRigidBodies();

	PBD::DistanceFieldCollisionDetection * cd = sim->get_collision_detection();
	std::vector<PBD::CollisionDetection::CollisionObject*> & collision_objects = cd->getCollisionObjects();

	rbs.clear();

	const int qty = this->get_child_count();
	for ( int i=0; i<qty; i++ )
	{
		Node * n = this->get_child( i );

		// Check if it is a rigid body.
		PbdRigidBodyNode * rbn = Object::cast_to<PbdRigidBodyNode>( n );
		if ( rbn )
		{
			// Insert the rigid body into the simulation.
			PBD::RigidBody * body = rbn->get_rigid_body();
			rbs.push_back( body );

			PBD::DistanceFieldCollisionDetection::DistanceFieldCollisionObject * co = reinterpret_cast<PBD::DistanceFieldCollisionDetection::DistanceFieldCollisionObject *>( rbn->get_collision_object() );
			if ( co )
			{
				const int index = static_cast<int>( rbs.size() ) - 1;
				co->m_bodyIndex = index;
				collision_objects.push_back( co );
			}
		}

		// Check if it is a constraint.
		// ..... TODO .....
	}
}

void PbdSimulationNode::set_time_step( real_t h )
{
	if ( !sim )
		return;

	PBD::TimeManager * tm = sim->get_time_manager();
	tm->setTimeStepSize( h );
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






