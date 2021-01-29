
#ifndef __PBD2_SIMULATION_H_
#define __PBD2_SIMULATION_H_

#include "data_types.h"
#include "pbd2_broad_tree.h"
#include "pbd2_contact_point_bb.h"

#include "core/vector.h"

using namespace Ign;

namespace Pbd
{

class RigidBody;
class Joint;

class Simulation
{
public:
    Simulation();
    ~Simulation();

	void set_broad_phase_max_depth( int depth );
	int  get_broad_phase_max_depth() const;
    
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
    BroadTree           tree;
    Vector<ContactPointBb> contacts;
    Vector<ContactPointBb> contacts_all;
    Vector<int>            contacts_all_qtys;

    //int step_number;

    static bool solve_normal( RigidBody * body_a, RigidBody * body_b, Vector<ContactPointBb> & pts, Float h );
    static void solve_tangential( RigidBody * body_a, RigidBody * body_b, Vector<ContactPointBb> & pts, Float h );
    static void solve_dynamic_friction( Simulation * sim, int base_ind, int qty, Float h );
    static bool specific_mass_pos( bool is_a, RigidBody * body, const Vector<ContactPointBb> & pts, Float & w );

	static void store_body_states( Vector<RigidBody *> & bodies );
	static void solve_simple_all( Vector<ContactPointBb> & pts, Float h );
	static void solve_dynamic_friction_simple_all( Vector<ContactPointBb> & pts, Float h );
};




}


#endif



