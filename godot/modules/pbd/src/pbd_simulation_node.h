
#ifndef __PBD_SIMULATION_NODE_H_
#define __PBD_SIMULATION_NODE_H_

#include "scene/3d/spatial.h"
#include "Common.h"

class PbdSimulationRid;

class PbdSimulationNode: public Spatial
{
	GDCLASS( PbdSimulationNode, Spatial );

public:
	PbdSimulationNode();
	~PbdSimulationNode();

protected:
	void _notification( int p_what );
	static void _bind_methods();

public:
	// This one iterates over direct children and inserts
	// them into simulation if those are
	// rigid bodies or constraints (joints).
	void start();
	void set_time_step( real_t h );
	void step( real_t dt );

	RID sim_rid;
	PbdSimulationRid * sim;
	Real t;

private:
	bool init();
	void finit();
};



#endif


