
#ifndef __PBD2_SIMULATION_H_
#define __PBD2_SIMULATION_H_

#include "data_types.h"
#include "core/vector.h"

using namespace Ign;

namespace Pbd
{

class Simulation
{
public:
    Simulation();
    ~Simulation();
    
    void set_time_step( Float h );
    void set_solver_iterations( int qty );

    void step( Float delta );
    void step();

    void clear();
    void add_body( RigidBody * body );
    void add_joint( Joint * joint );

    Float h, time_remainder;
    int solver_iterations;
    Vector<RigidBody *> bodies;
    Vector<Joint *>     joints;
};




}


#endif



