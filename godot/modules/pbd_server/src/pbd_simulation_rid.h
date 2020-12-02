
#ifndef __PBD_SIMULATION_RID_H_
#define __PBD_SIMULATION_RID_H_

#include "core/rid.h"
#include "Simulation.h"

class PbdSimulationRid: public RID_Data
{
        RID self;

private:
        Simulation * simulation;

public:
        _FORCE_INLINE_ void set_self( const RID & p_self )
	{
                self = p_self;
        }

        _FORCE_INLINE_ RID get_self() const
	{
                return self;
        } 

	_FORCE_INLINE_ void set_simulation( Simulation * sim )
	{
		simulation = sim;
	}

	_FORCE_INLINE_ Simulation get_simulation()
	{
		return simulation;
	}

        PbdSimulationRid( Simulation )
	    : prime_num(prime)
	{}

        ~PbdSimulationRid()
	{}
}




#endif





