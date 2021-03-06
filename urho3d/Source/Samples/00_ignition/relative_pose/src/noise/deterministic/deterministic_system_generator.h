
#ifndef __DETERMINISTIC_SYSTEM_GENERATOR_H_
#define __DETERMINISTIC_SYSTEM_GENERATOR_H_

#include "data_types.h"
#include "pi_system.h"
#include "pi_random.h"
#include "pi_source.h"
#include "deterministic_source.h"

namespace Ign
{

class DeterministicSystemGenerator
{
public:
    DeterministicSystemGenerator();
    ~DeterministicSystemGenerator();

    void generate_root_star( PiSystem * system, PiRandom & rand );
    void generate_single_planet( PiSystem * system, PiRandom & rand );
    void generate_moon( PiSystem * system, PiRandom & rand );
    void generate_system( PiSystem * system, PiRandom & rand );

    static DeterministicSource * heightSource( const PiSourceDesc & bodyDesc, int bodyIndex );
};

}



#endif




