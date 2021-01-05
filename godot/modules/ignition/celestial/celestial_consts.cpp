
#include "celestial_consts.h"


namespace Ign
{


const Float Celestial::EPS       = 0.0001;
const int   Celestial::MAX_ITERS = 64;
const Float Celestial::MIN_ANGULAR_MOMENTUM = 0.0001;

static const Celestial::Ticks TICKS_PER_SEC = 1000;

Celestial::Ticks Celestial::secs_to_ticks( Float secs )
{
    const Ticks ticks = static_cast<Ticks>( secs * static_cast<Float>(TICKS_PER_SEC) );
    return ticks;
}

Float Celestial::ticks_to_secs( Celestial::Ticks ticks )
{
    const Float secs = static_cast<Float>( static_cast<Float>(ticks) / static_cast<Float>(TICKS_PER_SEC) );
    return secs;
}






}






