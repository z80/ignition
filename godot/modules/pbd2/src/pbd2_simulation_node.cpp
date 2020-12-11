
#include "pbd2_simulation_node.h"

namespace Pbd
{

PbdSimulationNode::PbdSimulationNode()
    : Spatial()
{
}

PbdSimulationNode::~PbdSimulationNode()
{
}

void PbdSimulationNode::set_time_step( real_t h )
{
    simulation.h = h;
}

real_t PbdSimulationNode::get_time_step() const
{
    return simulation.h;
}

void PbdSimulationNode::set_solver_iterations( int qty )
{
    simulation.solver_iterations = qty;
}

int PbdSimulationNode::get_solver_iterations() const
{
    return simulation.solver_iterations;
}

void PbdSimulationNode::step( real_t delta )
{
    simulation.step( delta );
}

void PbdSimulationNode::one_step()
{
    simulation.step();
}

void PbdSimulationNode::_bind_methods()
{
    ClassDB::bind_method( D_METHOD( "set_time_step", "h" ), &PbdRigidBodyNode::set_time_step );
    ClassDB::bind_method( D_METHOD( "get_time_step" ),      &PbdRigidBodyNode::get_time_step, Variant::REAL );

    ClassDB::bind_method( D_METHOD( "set_solver_iterations", "h" ), &PbdRigidBodyNode::set_solver_iterations );
    ClassDB::bind_method( D_METHOD( "get_solver_iterations" ),      &PbdRigidBodyNode::get_solver_iterations, Variant::REAL );

    ClassDB::bind_method( D_METHOD( "step", "delta" ), &PbdRigidBodyNode::step );
    ClassDB::bind_method( D_METHOD( "one_step" ),      &PbdRigidBodyNode::one_step );


    ADD_PROPERTY( PropertyInfo( Variant::REAL, "time_step" ),         &PbdRigidBodyNode::set_time_step, &PbdRigidBodyNode::get_time_step );
    ADD_PROPERTY( PropertyInfo( Variant::INT,  "solver_iterations" ), &PbdRigidBodyNode::set_solver_iterations, &PbdRigidBodyNode::get_solver_iterations );
}




}










