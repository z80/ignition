
#include "pbd2_simulation.h"
#include "pbd2_rigid_body.h"
#include "pbd2_joint.h"

namespace Pbd
{


Simulation::Simulation()
{
    h = 0.01;
    solver_iterations = 3;
    time_remainder = 0.0;
}

Simulation::~Simulation()
{
}
    
void Simulation::set_time_step( Float h )
{
    this->h = h;
}

void Simulation::set_solver_iterations( int qty )
{
    solver_iterations = qty;
}

void Simulation::step( Float delta )
{
    Float t = time_remainder + delta;
    while ( t > 0.0 )
    {
        step();
        t -= h;
    }
    time_remainder = t;
}

void Simulation::step()
{
    // Integrate dynamics.
    const int bodies_qty = bodies.size();
    for ( int i=0; i<bodies_qty; i++ )
    {
        RigidBody * body = bodies.ptr()[i];
        body->integrate_dynamics( h );
    }

    // Initialize joint lambdas.
    const int joints_qty = joints.size();
    for ( int i=0; i<joints_qty; i++ )
    {
        Joint * joint = joints.ptr()[i];
        joint->init_lambdas();
    }
    
    // Initialize contact lambdas.
    for ( int i=0; i<bodies_qty; i++ )
    {
        // Here contact point solver lambdas init should go.
        RigidBody * body = bodies.ptr()[i];
        body->init_contact_lambdas();
    }

    // Solve joint and contact constraints.
    for ( int i=0; i<solver_iterations; i++ )
    {
        for ( int j=0; j<joints_qty; j++ )
        {
            Joint * joint = joints.ptr()[j];
            joint->solver_step( h );
        }

        for ( int j=0; j<bodies_qty; j++ )
        {
            RigidBody * body = bodies.ptr()[j];
            body->solve_contacts( h );
        }
    }

    // Update velocities.
    for ( int i=0; i<bodies_qty; i++ )
    {
        RigidBody * body = bodies.ptr()[i];
        body->update_velocities( h );
    }

    // Update contact velocities.
    for ( int i=0; i<bodies_qty; i++ )
    {
        RigidBody * body = bodies.ptr()[i];
        //body->update_contact_velocities( h );
    }

	// Update contact prev. positions.
	for ( int i=0; i<bodies_qty; i++ )
	{
		RigidBody * body = bodies.ptr()[i];
		body->update_contact_positions();
	}
}

void Simulation::clear()
{
    bodies.clear();
    joints.clear();
}

void Simulation::add_body( RigidBody * body )
{
    bodies.push_back( body );
}

void Simulation::add_joint( Joint * joint )
{
    joints.push_back( joint );
}


}







