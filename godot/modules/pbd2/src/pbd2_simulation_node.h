
#ifndef __PBD2_SIMULATION_NODE_H_
#define __PBD2_SIMULATION_NODE_H_

#include "pbd2_simulation.h"

namespace Pbd
{

class PbdSimulationNode: public Spatial
{
    GDCLASS( PbdSimulationNode, Spatial );

public:
    PbdSimulationNode();
    ~PbdSimulationNode();

    void set_time_step( real_t h );
    real_t get_time_step() const;

    void set_solver_iterations( int qty );
    int get_solver_iterations() const;

    void step( real_t delta );
    void one_step();

protected:
    static void _bind_methods();

    Simulation simulation;
};

}





#endif




