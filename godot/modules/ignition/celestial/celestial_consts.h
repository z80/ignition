
#ifndef __CELESTIAL_CONSTS_H_
#define __CELESTIAL_CONSTS_H_

#include "data_types.h"

namespace Ign
{

class Celestial
{
public:
    typedef signed long long Ticks;
    // For "dt" adjustments.
    static Ticks secs_to_ticks( Float secs );
    static Float ticks_to_secs( Ticks ticks );

    static const Float EPS;
    static const int   MAX_ITERS;
    static const Float MIN_ANGULAR_MOMENTUM;
};


}


#endif



