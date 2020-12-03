
#ifndef __PBD_SIMULATION_NODE_H_
#define __PBD_SIMULATION_NODE_H_

#include "scene/3d/spatial.h"

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
	void step();

	RID sim_rid;
	PbdSimulationRid * sim;

private:
	bool init();
	void finit();
};



#endif


