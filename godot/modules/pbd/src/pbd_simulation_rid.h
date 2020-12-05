
#ifndef __PBD_SIMULATION_RID_H_
#define __PBD_SIMULATION_RID_H_

#include "core/rid.h"

namespace PBD
{
	class Simulation;
	class TimeManager;
	class DistanceFieldCollisionDetection;
}

class PbdSimulationRid : public RID_Data
{
	RID self;

private:
	PBD::Simulation  * simulation;
	PBD::TimeManager * time_manager;
	PBD::DistanceFieldCollisionDetection * collision_detection;

public:
	_FORCE_INLINE_ void set_self(const RID & p_self)
	{
		self = p_self;
	}

	_FORCE_INLINE_ RID get_self() const
	{
		return self;
	}

	_FORCE_INLINE_ void set_simulation( PBD::Simulation * sim )
	{
		simulation = sim;
	}

	_FORCE_INLINE_ PBD::Simulation * get_simulation()
	{
		return simulation;
	}

	_FORCE_INLINE_ void set_time_manager( PBD::TimeManager * tm )
	{
		time_manager = tm;
	}

	_FORCE_INLINE_ PBD::TimeManager * get_time_manager()
	{
		return time_manager;
	}

	_FORCE_INLINE_ PBD::DistanceFieldCollisionDetection * get_collision_detection()
	{
		return collision_detection;
	}

	PbdSimulationRid();
	~PbdSimulationRid();
};




#endif





