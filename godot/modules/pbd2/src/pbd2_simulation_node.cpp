
#include "pbd2_simulation_node.h"
#include "pbd2_rigid_body_node.h"

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

PoolVector3Array PbdSimulationNode::lines_nodes() const
{
    const PoolVector3Array ret = simulation.tree.lines_nodes();
    return ret;
}

void PbdSimulationNode::_bind_methods()
{
    ClassDB::bind_method( D_METHOD( "set_time_step", "h" ), &PbdSimulationNode::set_time_step );
    ClassDB::bind_method( D_METHOD( "get_time_step" ),      &PbdSimulationNode::get_time_step, Variant::REAL );

    ClassDB::bind_method( D_METHOD( "set_solver_iterations", "h" ), &PbdSimulationNode::set_solver_iterations );
    ClassDB::bind_method( D_METHOD( "get_solver_iterations" ),      &PbdSimulationNode::get_solver_iterations, Variant::REAL );

    ClassDB::bind_method( D_METHOD( "step", "delta" ), &PbdSimulationNode::step );
    ClassDB::bind_method( D_METHOD( "one_step" ),      &PbdSimulationNode::one_step );

    ClassDB::bind_method( D_METHOD( "lines_nodes" ),   &PbdSimulationNode::lines_nodes );


    ADD_PROPERTY( PropertyInfo( Variant::REAL, "time_step" ),         "set_time_step", "get_time_step" );
    ADD_PROPERTY( PropertyInfo( Variant::INT,  "solver_iterations" ), "set_solver_iterations", "get_solver_iterations" );
}




}










