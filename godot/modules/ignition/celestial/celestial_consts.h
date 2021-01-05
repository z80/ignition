
#ifndef __CELESTIAL_CONSTS_H_
#define __CELESTIAL_CONSTS_H_

#include "data_types.h"

namespace Ign
{

namespace Celestial
{
    typename Ticks int64_t;
    // For "dt" adjustments.
    Ticks secs_to_tics( Float secs );
    Float ticks_to_secs( Ticks ticks );

    static const Float EPS;
    static const int   MAX_ITERS;
    static const Float MIN_ANGULAR_MOMENTUM;
};


}


#endif



