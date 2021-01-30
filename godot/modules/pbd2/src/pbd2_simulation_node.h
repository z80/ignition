
#ifndef __PBD2_SIMULATION_NODE_H_
#define __PBD2_SIMULATION_NODE_H_

#include "scene/3d/spatial.h"
#include "pbd2_simulation.h"

namespace Pbd
{

class PbdSimulationNode: public Spatial
{
    GDCLASS( PbdSimulationNode, Spatial );

public:
    PbdSimulationNode();
    ~PbdSimulationNode();

	void set_broad_phase_max_depth( int depth );
	int get_broad_phase_max_depth() const;

    void set_time_step( real_t h );
    real_t get_time_step() const;

    void set_solver_iterations( int qty );
    int get_solver_iterations() const;

	void set_contact_erp( real_t k );
	real_t get_contact_erp() const;

    void step( real_t delta );
    void one_step();

    PoolVector3Array lines_nodes() const;

protected:
    static void _bind_methods();

public:
    Simulation simulation;
};

}





#endif




