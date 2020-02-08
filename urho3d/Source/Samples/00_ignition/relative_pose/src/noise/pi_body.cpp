
#include "pi_body.h"

namespace Ign
{

const int PiBodyDesc::MAX_VALUE = 1000;

PiBodyDesc::PiBodyDesc()
{
    seed_         = 0;
    GM_           = 1.0;

    life_         = 10;
    gas_          = 50;
    average_temp_ = 240;
    liquid_       = 200;
    volcanic_     = 50;
    ice_          = 10;
    metal_        = 10;
}

PiBodySource::PiBodySource
    : HeightSource()
{

}


}















